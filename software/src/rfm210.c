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
	XMC_GPIO_SetOutputHigh(P0_0);
	// Always write changes from low to high in even array index
	// and changes from high to low in odd array index.
	// If the edge changed too fast for the interrupt, we will the missed
	// index up with the current time too.
	if(( XMC_GPIO_GetInput(RFM210_RECV_PIN) && (  rfm210.timestamp_end & 1)) ||
	   (!XMC_GPIO_GetInput(RFM210_RECV_PIN) && (!(rfm210.timestamp_end & 1)))) {
		rfm210.timestamps[rfm210.timestamp_end] = CCU40_CC41->TIMER;
		rfm210.timestamp_end = (rfm210.timestamp_end+1) & RFM210_TIMESTAMP_MASK;
	}
	rfm210.timestamps[rfm210.timestamp_end] = CCU40_CC41->TIMER;
	rfm210.timestamp_end = (rfm210.timestamp_end+1) & RFM210_TIMESTAMP_MASK;
	XMC_GPIO_SetOutputLow(P0_0);
}

void rfm210_init(RFM210 *rfm210) {
	// Testing

	const XMC_GPIO_CONFIG_t recv_toggle_pin_config = {
		.mode         = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};
	XMC_GPIO_Init(P0_0, &recv_toggle_pin_config);
	XMC_GPIO_Init(P0_1, &recv_toggle_pin_config);

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
    XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, 480);
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

uint8_t rfm210_crc8(const uint8_t *data, const uint8_t length) {
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

char *direction_name[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};

void rfm210_print_packet(RFM210Packet *packet) {
	logd("RFM210 packet:\n\r");
	logd(" preamble:             %u\n\r", packet->preamble);
	logd(" family_code:          %u\n\r", packet->family_code);
	logd(" device_id:            %u\n\r", packet->device_id);
	logd(" temperature:          %u\n\r", packet->temperature);           // -400
	logd(" temperature_flag1:    %u\n\r", packet->temperature_flag1);
	logd(" temperature_flag2:    %u\n\r", packet->temperature_flag2);
	logd(" humidity:             %u\n\r", packet->humidity);
	logd(" wind_speed:           %u\n\r", packet->wind_speed);            // *34/100 m/s
	logd(" gust:                 %u\n\r", packet->gust);                  // *34/100 m/s
	logd(" rain_counter:         %u\n\r", packet->rain_counter);          // *0.3 ???
	logd(" wind_direction_error: %u\n\r", packet->wind_direction_error);
	logd(" low_battery:          %u\n\r", packet->low_battery);
	logd(" wind_direction:       %u\n\r", packet->wind_direction);
	logd(" crc:                  %u\n\r", packet->crc);
	logd("\n\r");
}

void rfm210_reset(RFM210 *rfm210) {
	memset(rfm210->data, 0, RFM210_DATA_SIZE);

	rfm210->data_index = 0;
	rfm210->data_bit   = 1;
	rfm210->data[0]    = 1 << 7;
}

#define NIBBLE_LOW(value) ((value >> 4) & 0xF)
#define NIBBLE_HIGH(value) (value & 0xF)
void rfm210_check_data(RFM210 *rfm210) {
	RFM210Packet *packet         = &rfm210->packet;
	packet->preamble             = (NIBBLE_HIGH(rfm210->data[0]) << 4)         |  NIBBLE_LOW(rfm210->data[0]);
	packet->family_code          =  NIBBLE_LOW(rfm210->data[1]);
	packet->device_id            = (NIBBLE_HIGH(rfm210->data[1]) << 4)         |  NIBBLE_LOW(rfm210->data[2]);
	packet->temperature          = ((NIBBLE_HIGH(rfm210->data[2]) & 0x3) << 8) | (NIBBLE_LOW(rfm210->data[3])  << 4) |  NIBBLE_HIGH(rfm210->data[3]);
	packet->temperature_flag1    = NIBBLE_HIGH(rfm210->data[2]) & (1 << 3);
	packet->temperature_flag2    = NIBBLE_HIGH(rfm210->data[2]) & (1 << 2);
	packet->humidity             = (NIBBLE_LOW(rfm210->data[4]) << 4)          |  NIBBLE_HIGH(rfm210->data[4]);
	packet->wind_speed           = (NIBBLE_LOW(rfm210->data[5]) << 4)          |  NIBBLE_HIGH(rfm210->data[5]);
	packet->gust                 = (NIBBLE_LOW(rfm210->data[6]) << 4)          |  NIBBLE_HIGH(rfm210->data[6]);
	packet->rain_counter         = (NIBBLE_LOW(rfm210->data[7]) << 12)         | (NIBBLE_HIGH(rfm210->data[7]) << 8) | (NIBBLE_LOW(rfm210->data[8]) << 4) | NIBBLE_HIGH(rfm210->data[8]);
	packet->wind_direction_error =  NIBBLE_LOW(rfm210->data[9]) & (1 << 3);
	packet->low_battery          =  NIBBLE_LOW(rfm210->data[9]) & (1 << 0);
	packet->wind_direction       =  NIBBLE_HIGH(rfm210->data[9]);
	packet->crc                  = (NIBBLE_LOW(rfm210->data[10]) << 4)         |  NIBBLE_HIGH(rfm210->data[10]);

	logd("data: "); log_array_u8(rfm210->data, 11, true);
	logd("crc: %u\n\r", rfm210_crc8(&rfm210->data[1], 9));
	rfm210_print_packet(packet);
}

uint32_t rfm210_get_next_timestamp_index(RFM210 *rfm210, uint32_t index) {
	return (index + 1) & RFM210_TIMESTAMP_MASK;
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
			if((rfm210->data_bit != 7) || (rfm210->data_index != 10)) {
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
			XMC_GPIO_SetOutputHigh(P0_1);
			rfm210->data_index++;
			rfm210->data_bit = 0;
			if(rfm210->data_index >= 11) {
				rfm210_check_data(rfm210);
				rfm210_reset(rfm210);
			}
		}

		rfm210->timestamp_start = nextnext;
	}
	XMC_GPIO_SetOutputLow(P0_1);

#if 0
	if(rfm210->timestamp_start != rfm210->timestamp_end) {
		uint32_t last = (rfm210->timestamp_start-1) & RFM210_TIMESTAMP_MASK;
		uint32_t difference = (rfm210->timestamps[rfm210->timestamp_start] - rfm210->timestamps[last]) & 0xFFFF;
		if(difference > 40 && difference < 160) {
			logd("delta %u: %u\n\r", rfm210->timestamp_start, difference);
		}
		rfm210->timestamp_start = (rfm210->timestamp_start + 1) & RFM210_TIMESTAMP_MASK;
	} else {
		break;
	}
#endif
}
