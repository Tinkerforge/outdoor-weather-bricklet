/* outdoor-weather-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants
#define OUTDOOR_WEATHER_WIND_DIRECTION_N 0
#define OUTDOOR_WEATHER_WIND_DIRECTION_NNE 1
#define OUTDOOR_WEATHER_WIND_DIRECTION_NE 2
#define OUTDOOR_WEATHER_WIND_DIRECTION_ENE 3
#define OUTDOOR_WEATHER_WIND_DIRECTION_E 4
#define OUTDOOR_WEATHER_WIND_DIRECTION_ESE 5
#define OUTDOOR_WEATHER_WIND_DIRECTION_SE 6
#define OUTDOOR_WEATHER_WIND_DIRECTION_SSE 7
#define OUTDOOR_WEATHER_WIND_DIRECTION_S 8
#define OUTDOOR_WEATHER_WIND_DIRECTION_SSW 9
#define OUTDOOR_WEATHER_WIND_DIRECTION_SW 10
#define OUTDOOR_WEATHER_WIND_DIRECTION_WSW 11
#define OUTDOOR_WEATHER_WIND_DIRECTION_W 12
#define OUTDOOR_WEATHER_WIND_DIRECTION_WNW 13
#define OUTDOOR_WEATHER_WIND_DIRECTION_NW 14
#define OUTDOOR_WEATHER_WIND_DIRECTION_NNW 15
#define OUTDOOR_WEATHER_WIND_DIRECTION_ERROR 255

#define OUTDOOR_WEATHER_BOOTLOADER_MODE_BOOTLOADER 0
#define OUTDOOR_WEATHER_BOOTLOADER_MODE_FIRMWARE 1
#define OUTDOOR_WEATHER_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define OUTDOOR_WEATHER_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define OUTDOOR_WEATHER_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define OUTDOOR_WEATHER_BOOTLOADER_STATUS_OK 0
#define OUTDOOR_WEATHER_BOOTLOADER_STATUS_INVALID_MODE 1
#define OUTDOOR_WEATHER_BOOTLOADER_STATUS_NO_CHANGE 2
#define OUTDOOR_WEATHER_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define OUTDOOR_WEATHER_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define OUTDOOR_WEATHER_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define OUTDOOR_WEATHER_STATUS_LED_CONFIG_OFF 0
#define OUTDOOR_WEATHER_STATUS_LED_CONFIG_ON 1
#define OUTDOOR_WEATHER_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define OUTDOOR_WEATHER_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_GET_STATION_IDENTIFIERS_LOW_LEVEL 1
#define FID_GET_SENSOR_IDENTIFIERS_LOW_LEVEL 2
#define FID_GET_STATION_DATA 3
#define FID_GET_SENSOR_DATA 4
#define FID_SET_STATION_CALLBACK_CONFIGURATION 5
#define FID_GET_STATION_CALLBACK_CONFIGURATION 6
#define FID_SET_SENSOR_CALLBACK_CONFIGURATION 7
#define FID_GET_SENSOR_CALLBACK_CONFIGURATION 8

#define FID_CALLBACK_STATION_DATA 9
#define FID_CALLBACK_SENSOR_DATA 10

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetStationIdentifiersLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint16_t identifiers_length;
	uint16_t identifiers_chunk_offset;
	uint8_t identifiers_chunk_data[60];
} __attribute__((__packed__)) GetStationIdentifiersLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetSensorIdentifiersLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint16_t identifiers_length;
	uint16_t identifiers_chunk_offset;
	uint8_t identifiers_chunk_data[60];
} __attribute__((__packed__)) GetSensorIdentifiersLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t identifier;
} __attribute__((__packed__)) GetStationData;

typedef struct {
	TFPMessageHeader header;
	int16_t temperature;
	uint8_t humidity;
	uint32_t wind_speed;
	uint32_t gust_speed;
	uint32_t rain;
	uint8_t wind_direction;
	bool battery_low;
	uint16_t last_change;
} __attribute__((__packed__)) GetStationData_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t identifier;
} __attribute__((__packed__)) GetSensorData;

typedef struct {
	TFPMessageHeader header;
	int16_t temperature;
	uint8_t humidity;
	uint16_t last_change;
} __attribute__((__packed__)) GetSensorData_Response;

typedef struct {
	TFPMessageHeader header;
	bool enable_callback;
} __attribute__((__packed__)) SetStationCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetStationCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	bool enable_callback;
} __attribute__((__packed__)) GetStationCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	bool enable_callback;
} __attribute__((__packed__)) SetSensorCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetSensorCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	bool enable_callback;
} __attribute__((__packed__)) GetSensorCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t identifier;
	int16_t temperature;
	uint8_t humidity;
	uint32_t wind_speed;
	uint32_t gust_speed;
	uint32_t rain;
	uint8_t wind_direction;
	bool battery_low;
} __attribute__((__packed__)) StationData_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t identifier;
	int16_t temperature;
	uint8_t humidity;
} __attribute__((__packed__)) SensorData_Callback;


// Function prototypes
BootloaderHandleMessageResponse get_station_identifiers_low_level(const GetStationIdentifiersLowLevel *data, GetStationIdentifiersLowLevel_Response *response);
BootloaderHandleMessageResponse get_sensor_identifiers_low_level(const GetSensorIdentifiersLowLevel *data, GetSensorIdentifiersLowLevel_Response *response);
BootloaderHandleMessageResponse get_station_data(const GetStationData *data, GetStationData_Response *response);
BootloaderHandleMessageResponse get_sensor_data(const GetSensorData *data, GetSensorData_Response *response);
BootloaderHandleMessageResponse set_station_callback_configuration(const SetStationCallbackConfiguration *data);
BootloaderHandleMessageResponse get_station_callback_configuration(const GetStationCallbackConfiguration *data, GetStationCallbackConfiguration_Response *response);
BootloaderHandleMessageResponse set_sensor_callback_configuration(const SetSensorCallbackConfiguration *data);
BootloaderHandleMessageResponse get_sensor_callback_configuration(const GetSensorCallbackConfiguration *data, GetSensorCallbackConfiguration_Response *response);

// Callbacks
bool handle_station_data_callback(void);
bool handle_sensor_data_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 2
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_station_data_callback, \
	handle_sensor_data_callback, \


#endif
