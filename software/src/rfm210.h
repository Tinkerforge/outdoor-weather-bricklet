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


#define RFM210_MAX_DEVICES 256
#define RFM210_DATA_SIZE 11
#define RFM210_DATA_PAYLOAD_SIZE 8
#define RFM210_TIMESTAMP_SIZE 2048
#define RFM210_TIMESTAMP_MASK (RFM210_TIMESTAMP_SIZE-1) // Note: Always use power of 2 here

typedef struct {
	uint8_t preamble;
	uint8_t family_code;
	uint8_t device_id;
	uint16_t temperature;
	uint16_t temperature_flag1;
	uint16_t temperature_flag2;
	uint8_t humidity;
	uint8_t wind_speed;
	uint8_t gust;
	uint16_t rain_counter;
	bool wind_direction_error;
	bool low_battery;
	uint8_t wind_direction;
	uint8_t crc;
} RFM210Packet;

typedef struct {
	uint16_t timestamps[RFM210_TIMESTAMP_SIZE];
	uint32_t timestamp_start;
	uint32_t timestamp_end;

	uint8_t data[RFM210_DATA_SIZE];
	uint32_t data_index;
	uint32_t data_bit;

	uint8_t payload[RFM210_MAX_DEVICES][RFM210_DATA_PAYLOAD_SIZE];

	RFM210Packet packet;
} RFM210;

void rfm210_init(RFM210 *rfm210);
void rfm210_tick(RFM210 *rfm210);

#endif
