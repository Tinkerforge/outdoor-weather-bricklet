/* outdoor-weather-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rfm210.h: RFM210 driver
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

#ifndef RFM210_H
#define RFM210_H

#include <stdint.h>
#include <stdbool.h>

#define RFM210_STATION_FAMILY_CODE 0b1010
#define RFM210_SENSOR_FAMILY_CODE 0b0100
#define RFM210_MAX_DEVICES 256
#define RFM210_DATA_SIZE 11
#define RFM210_DATA_PAYLOAD_STATION_SIZE 8
#define RFM210_DATA_PAYLOAD_SENSOR_SIZE 3
#define RFM210_TIMESTAMP_SIZE 2048
#define RFM210_TIMESTAMP_MASK (RFM210_TIMESTAMP_SIZE-1) // Note: Always use power of 2 here

#define RFM210_MIN_MESSAGE_GAP_MS 500

typedef struct {
	int16_t temperature;
	uint8_t humidity;
	uint32_t wind_speed;
	uint32_t gust_speed;
	uint32_t rain;
	bool battery_low;
	uint8_t wind_direction;
} RFM210PacketStation;

typedef struct {
	int16_t temperature;
	uint8_t humidity;
} RFM210PacketSensor;

typedef struct {
	uint16_t timestamps[RFM210_TIMESTAMP_SIZE];
	uint32_t timestamp_start;
	uint32_t timestamp_end;

	uint8_t data[RFM210_DATA_SIZE];
	uint32_t data_index;
	uint32_t data_bit;

	uint8_t payload_station[RFM210_MAX_DEVICES][RFM210_DATA_PAYLOAD_STATION_SIZE];
	uint32_t payload_station_last_change[RFM210_MAX_DEVICES];

	uint8_t payload_sensor[RFM210_MAX_DEVICES][RFM210_DATA_PAYLOAD_SENSOR_SIZE];
	uint32_t payload_sensor_last_change[RFM210_MAX_DEVICES];

	RFM210PacketStation packet_station;
	RFM210PacketSensor packet_sensor;

	uint16_t current_id_index_station;
	uint16_t current_chunk_offset_station;
	uint16_t current_id_index_sensor;
	uint16_t current_chunk_offset_sensor;

	bool callback_enabled_station;
	bool callback_enabled_sensor;
} RFM210;

bool rfm210_check_station_timeout(const uint8_t id);
bool rfm210_check_sensor_timeout(const uint8_t id);

void rfm210_fill_packet_station(RFM210 *rfm210, const uint16_t id, RFM210PacketStation *packet);
void rfm210_fill_packet_sensor(RFM210 *rfm210, const uint16_t id, RFM210PacketSensor *packet);
void rfm210_init(RFM210 *rfm210);
void rfm210_tick(RFM210 *rfm210);

#endif
