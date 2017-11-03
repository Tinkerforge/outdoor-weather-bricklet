/* outdoor-weather-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rfm210.c: RFM210 driver
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "rfm210.h"

#include <stdint.h>

#include "configs/config_rfm210.h"
#include "bricklib2/logging/logging.h"

#include "xmc_gpio.h"
#include "xmc_eru.h"
#include "xmc_ccu4.h"

#include "communication.h"


// OOK format:
// High:   |^|__| -> 500us  | 1000us
// Low:  |^^^|__| -> 1500us | 1000us


#define RFM210_TIME_DELTA   15 // We allow for a time delta of 150us
#define RFM210_OOK_ON_HIGH  50 // OOK on-time for high is 500us
#define RFM210_OOK_ON_LOW  150 // OOK on-time for low is 1500us
#define RFM210_OOK_OFF     100 // OOK off-time is 1000us (for low and high)


#define rfm210_recv_handler IRQ_Hdlr_3

extern RFM210 rfm210;
void __attribute__((optimize("-O3"))) __attribute__ ((section (".ram_code"))) rfm210_recv_handler(void) {
	// Always write changes from low to high in even array index
	// and changes from high to low in odd array index.
	// If the edge changed too fast for the interrupt, we will fill the missed
	// index up with the current time too.
	if(( XMC_GPIO_GetInput(RFM210_RECV_PIN) && (  rfm210.timestamp_end & 1)) ||
	   (!XMC_GPIO_GetInput(RFM210_RECV_PIN) && (!(rfm210.timestamp_end & 1)))) {
		rfm210.timestamps[rfm210.timestamp_end] = CCU40_CC41->TIMER;
		rfm210.timestamp_end = (rfm210.timestamp_end+1) & RFM210_TIMESTAMP_MASK;
	}
	rfm210.timestamps[rfm210.timestamp_end] = CCU40_CC41->TIMER;
	rfm210.timestamp_end = (rfm210.timestamp_end+1) & RFM210_TIMESTAMP_MASK;
}



static uint8_t rfm210_crc8(const uint8_t *data, const uint8_t length) {
	uint8_t crc = 0;

	for(uint8_t i = 0; i < length; i++) {
		uint8_t byte = data[i];
		for(uint8_t j = 0; j < 8; j++) {
			bool mix = (crc ^ byte) & 0x80;
			crc <<= 1;
			if(mix) {
				crc ^= 0x31;
			}
			byte <<= 1;
		}
	}

	return crc;
}

static void rfm210_print_packet_station(RFM210PacketStation *packet, uint8_t id) {
	logd("RFM210 station packet (%d):\n\r", id);
	logd(" temperature:          %u\n\r", packet->temperature);
	logd(" humidity:             %u\n\r", packet->humidity);
	logd(" wind_speed:           %u\n\r", packet->wind_speed);
	logd(" gust_speed:           %u\n\r", packet->gust_speed);
	logd(" rain                  %u\n\r", packet->rain);
	logd(" battery_low:          %u\n\r", packet->battery_low);
	logd(" wind_direction:       %u\n\r", packet->wind_direction);
	logd("\n\r");
}

static void rfm210_print_packet_sensor(RFM210PacketSensor *packet, uint8_t id) {
	logd("RFM210 sensor packet (%d):\n\r", id);
	logd(" temperature:          %u\n\r", packet->temperature);
	logd(" humidity:             %u\n\r", packet->humidity);
	logd("\n\r");
}

static void rfm210_reset(RFM210 *rfm210) {
	memset(rfm210->data, 0, RFM210_DATA_SIZE);

	rfm210->data_index = 0;
	rfm210->data_bit   = 1;
	rfm210->data[0]    = 1 << 7;
}

static uint32_t rfm210_get_next_timestamp_index(RFM210 *rfm210, uint32_t index) {
	return (index + 1) & RFM210_TIMESTAMP_MASK;
}

#define NIBBLE_LOW(value) ((value >> 4) & 0xF)
#define NIBBLE_HIGH(value) (value & 0xF)
static void rfm210_check_data_station(RFM210 *rfm210) {
//	logd("data ws: "); log_array_u8(rfm210->data, 11, true);
//	logd("crc: %d\n\r", rfm210_crc8(&rfm210->data[1], 9));

	// Check preamble, family code and CRC
	if((rfm210->data[0] != 0xFF) ||
	   ((rfm210->data[1] >> 4) != RFM210_STATION_FAMILY_CODE) ||
	   (rfm210_crc8(&rfm210->data[1], 9) != rfm210->data[10])) {
		return;
	}

	const uint8_t id = (NIBBLE_HIGH(rfm210->data[1]) << 4) | NIBBLE_LOW(rfm210->data[2]);

	memcpy(&rfm210->payload_station[id][0], &rfm210->data[2], 8);
	rfm210->payload_station_last_change[id] = system_timer_get_ms();

	// Testing
	RFM210PacketStation packet;
	rfm210_fill_packet_station(rfm210, id, &packet);
	rfm210_print_packet_station(&packet, id);
}

static bool rfm210_check_data_sensor(RFM210 *rfm210) {
//	logd("data ht: "); log_array_u8(rfm210->data, 7, true);
//	logd("crc: %d\n\r", rfm210_crc8(&rfm210->data[1], 4));

	// Check preamble, family code and CRC
	if((rfm210->data[0] != 0xFF) ||
	   ((rfm210->data[1] >> 4) != RFM210_SENSOR_FAMILY_CODE) ||
	   (rfm210_crc8(&rfm210->data[1], 4) != rfm210->data[5])) {
		return false;
	}

	const uint8_t id = (NIBBLE_HIGH(rfm210->data[1]) << 4) | NIBBLE_LOW(rfm210->data[2]);

	memcpy(&rfm210->payload_sensor[id][0], &rfm210->data[2], 3);
	rfm210->payload_sensor_last_change[id] = system_timer_get_ms();

	// Testing
	RFM210PacketSensor packet;
	rfm210_fill_packet_sensor(rfm210, id, &packet);
	rfm210_print_packet_sensor(&packet, id);

	return true;
}

void rfm210_fill_packet_station(RFM210 *rfm210, const uint16_t id, RFM210PacketStation *packet) {
	uint8_t *data = &rfm210->payload_station[id][0];

	packet->temperature       = ((NIBBLE_HIGH(data[0]) & 0x3) << 8) | (NIBBLE_LOW(data[1])  << 4) |  NIBBLE_HIGH(data[1]);
	packet->humidity          = (NIBBLE_LOW(data[2]) << 4)          |  NIBBLE_HIGH(data[2]);
	packet->wind_speed        = (NIBBLE_LOW(data[3]) << 4)          |  NIBBLE_HIGH(data[3]);
	packet->gust_speed        = (NIBBLE_LOW(data[4]) << 4)          |  NIBBLE_HIGH(data[4]);
	packet->rain              = (NIBBLE_LOW(data[5]) << 12)         | (NIBBLE_HIGH(data[5]) << 8) | (NIBBLE_LOW(data[6]) << 4) | NIBBLE_HIGH(data[6]);
	packet->battery_low       =  NIBBLE_LOW(data[7]) & (1 << 0);
	packet->wind_direction    =  NIBBLE_HIGH(data[7]);


	// TODO: What are these temperature flags and how can we handle them?
//	bool temperature_flag1    = NIBBLE_HIGH(rfm210->data[2]) & (1 << 3);
//	bool temperature_flag2    = NIBBLE_HIGH(rfm210->data[2]) & (1 << 2);
	bool wind_direction_error =  NIBBLE_LOW(data[7]) & (1 << 3);

	// Handle wind direction errors
	if(wind_direction_error || (packet->wind_direction > OUTDOOR_WEATHER_WIND_DIRECTION_NNW)) {
		packet->wind_direction = OUTDOOR_WEATHER_WIND_DIRECTION_ERROR;
	}

	// Convert values to proper SI units, humidity is already in %rel
	packet->temperature = packet->temperature - 400; // °C/10
	packet->wind_speed  = packet->wind_speed*340/100; // m/10s
	packet->gust_speed  = packet->gust_speed*340/100; // m/10s
	packet->rain        = packet->rain*30/10;         // mm/10
}

void rfm210_fill_packet_sensor(RFM210 *rfm210, const uint16_t id, RFM210PacketSensor *packet) {
	uint8_t *data = &rfm210->payload_sensor[id][0];

	packet->temperature     = ((NIBBLE_HIGH(data[0]) & 0x3) << 8) | (NIBBLE_LOW(data[1])  << 4) |  NIBBLE_HIGH(data[1]);
	packet->humidity        = (NIBBLE_LOW(data[2]) << 4)          |  NIBBLE_HIGH(data[2]);

	if(NIBBLE_HIGH(data[0]) & (1 << 3)) {
		packet->temperature = -packet->temperature;
	}
}

void rfm210_init(RFM210 *rfm210) {
	// ************** Data ****************
	memset(rfm210, 0, sizeof(RFM210));


	// ************** GPIO ****************
	const XMC_GPIO_CONFIG_t nreset_pin_config = {
		.mode         = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};
	XMC_GPIO_Init(RFM210_NENABLE_PIN, &nreset_pin_config);

	const XMC_GPIO_CONFIG_t recv_pin_config = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_LARGE
	};

	const XMC_ERU_ETL_CONFIG_t sync_etl_config = {
		.input_a                = (uint32_t)XMC_ERU_ETL_INPUT_A0,
		.input_b                = (uint32_t)XMC_ERU_ETL_INPUT_B0,
		.enable_output_trigger  = (uint32_t)1,
		.edge_detection         = XMC_ERU_ETL_EDGE_DETECTION_BOTH,
		.output_trigger_channel = XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL0, // Select the source for event
		.source                 = XMC_ERU_ETL_SOURCE_B
	};

	// Initializes input pin characteristics
	XMC_GPIO_Init(RFM210_RECV_PIN, &recv_pin_config);
	// ERU Event Trigger Logic Hardware initialization based on UI
	XMC_ERU_ETL_Init(XMC_ERU0, RFM210_RECV_ETL_CHANNEL, &sync_etl_config);
	// OGU is configured to generate event on configured trigger edge
	XMC_ERU_OGU_SetServiceRequestMode(XMC_ERU0, RFM210_RECV_OGU_CHANNEL, XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER);
	// Configure NVIC node and priority
	NVIC_SetPriority(RFM210_RECV_IRQ_N, RFM210_RECV_IRQ_PRIO);

	// Clear pending interrupt before enabling it
	NVIC_ClearPendingIRQ(RFM210_RECV_IRQ_N);
	// Enable NVIC node
	NVIC_EnableIRQ(RFM210_RECV_IRQ_N);


	// ************** Timer ****************

	XMC_CCU4_SLICE_COMPARE_CONFIG_t timer0_config = {
		.timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot            = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
		.shadow_xfer_clear   = false,
		.dither_timer_period = false,
		.dither_duty_cycle   = false,
		.prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable          = false,
		.prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_2,
		.float_limit         = 0U,
		.dither_limit        = 0U,
		.passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = false
	};

	XMC_CCU4_SLICE_COMPARE_CONFIG_t timer1_config = {
		.timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot            = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
		.shadow_xfer_clear   = false,
		.dither_timer_period = false,
		.dither_duty_cycle   = false,
		.prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable          = false,
		.prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_2,
		.float_limit         = 0U,
		.dither_limit        = 0U,
		.passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = true
	};

	// Initialize CCU4
    XMC_CCU4_Init(CCU40, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    XMC_CCU4_StartPrescaler(CCU40);

    // Slice 0: Count from 0 to 480 (10us) TODO: Change to 320 for smalle MCU
    XMC_CCU4_EnableClock(CCU40, 0);
    XMC_CCU4_SLICE_CompareInit(CCU40_CC40, &timer0_config);
    XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, 320);
    XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC40, 0);
    XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0);

    // Slice 1: Concatenate with Slice 0 to count from 0 to 0xFFFF with 10us resolution
    XMC_CCU4_EnableClock(CCU40, 1);
    XMC_CCU4_SLICE_CompareInit(CCU40_CC41, &timer1_config);
    XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC41, 0xFFFF);
    XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC41, 0);


    XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_1 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_1);

    // Start
    XMC_CCU4_SLICE_StartTimer(CCU40_CC40);
    XMC_CCU4_SLICE_StartTimer(CCU40_CC41);
}

void rfm210_tick(RFM210 *rfm210) {
	while(true) {
		uint32_t start = rfm210->timestamp_start;
		uint32_t end = rfm210->timestamp_end;
		uint32_t next = rfm210_get_next_timestamp_index(rfm210, start);
		if(next == end) {
			break;
		}

		if(start & 1) { // A bit always starts with even index
			rfm210->timestamp_start = next;
			continue;
		}

		uint32_t nextnext = rfm210_get_next_timestamp_index(rfm210, next);
		if(nextnext == end) {
			break;
		}

		uint32_t time_diff1 = (rfm210->timestamps[next] - rfm210->timestamps[start]) & 0xFFFF;
		uint32_t time_diff2 = (rfm210->timestamps[nextnext] - rfm210->timestamps[next]) & 0xFFFF;


		if((time_diff2 < RFM210_OOK_OFF-RFM210_TIME_DELTA) ||
		   (time_diff2 > RFM210_OOK_OFF+RFM210_TIME_DELTA)) {
			if((rfm210->data_bit != 7) || ((rfm210->data_index != 6) && (rfm210->data_index != 10))) {
				rfm210->timestamp_start = nextnext;
				rfm210_reset(rfm210);
				continue;
			}
		}

		if((time_diff1 > RFM210_OOK_ON_HIGH-RFM210_TIME_DELTA) &&
		   (time_diff1 < RFM210_OOK_ON_HIGH+RFM210_TIME_DELTA)) {
			rfm210->data[rfm210->data_index] |= (1 << (7-rfm210->data_bit));
			rfm210->data_bit++;
		} else if((time_diff1 > RFM210_OOK_ON_LOW-RFM210_TIME_DELTA) &&
				  (time_diff1 < RFM210_OOK_ON_LOW+RFM210_TIME_DELTA)) {
			rfm210->data_bit++;
		} else {
			rfm210->timestamp_start = nextnext;
			rfm210_reset(rfm210);
			continue;
		}

		if(rfm210->data_bit >= 8) {
			rfm210->data_index++;
			rfm210->data_bit = 0;

			// Check for sensor data
			if((rfm210->data_bit == 0) && (rfm210->data_index == 7)) {
				// Only reset if sensor data is found, otherwise this might be station data
				if(rfm210_check_data_sensor(rfm210)) {
					rfm210_reset(rfm210);
				}
			}

			// Check for station data
			if(rfm210->data_index >= 11) {
				rfm210_check_data_station(rfm210);
				rfm210_reset(rfm210);
			}
		}

		rfm210->timestamp_start = nextnext;
	}
}
