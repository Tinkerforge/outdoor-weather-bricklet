/* outdoor-weather-bricklet
 * Copyright (C) 2017 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/protocols/tfp/tfp.h"

#include "rfm210.h"

extern RFM210 rfm210;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_STATION_IDENTIFIERS_LOW_LEVEL: return get_station_identifiers_low_level(message, response);
		case FID_GET_SENSOR_IDENTIFIERS_LOW_LEVEL: return get_sensor_identifiers_low_level(message, response);
		case FID_GET_STATION_DATA: return get_station_data(message, response);
		case FID_GET_SENSOR_DATA: return get_sensor_data(message, response);
		case FID_SET_STATION_CALLBACK_CONFIGURATION: return set_station_callback_configuration(message);
		case FID_GET_STATION_CALLBACK_CONFIGURATION: return get_station_callback_configuration(message, response);
		case FID_SET_SENSOR_CALLBACK_CONFIGURATION: return set_sensor_callback_configuration(message);
		case FID_GET_SENSOR_CALLBACK_CONFIGURATION: return get_sensor_callback_configuration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse get_station_identifiers_low_level(const GetStationIdentifiersLowLevel *data, GetStationIdentifiersLowLevel_Response *response) {
	response->header.length = sizeof(GetStationIdentifiersLowLevel_Response);

	uint8_t response_index = 0;
	for(; rfm210.current_id_index_station < RFM210_MAX_DEVICES; rfm210.current_id_index_station++) {
		if(rfm210.payload_station_last_change[rfm210.current_id_index_station] != 0) {
			if(rfm210_check_station_timeout(rfm210.current_id_index_station)) {
				continue;
			}
			response->identifiers_chunk_data[response_index] = rfm210.current_id_index_station;
			response_index++;
			if(response_index == 60) {
				break;
			}
		}
	}

	response->identifiers_length = response_index;
	response->identifiers_chunk_offset = rfm210.current_chunk_offset_station;

	if(rfm210.current_id_index_station == RFM210_MAX_DEVICES) {
		rfm210.current_id_index_station = 0;
		rfm210.current_chunk_offset_station = 0;
	} else {
		rfm210.current_chunk_offset_station += response_index;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_sensor_identifiers_low_level(const GetSensorIdentifiersLowLevel *data, GetSensorIdentifiersLowLevel_Response *response) {
	response->header.length = sizeof(GetSensorIdentifiersLowLevel_Response);

	uint8_t response_index = 0;
	for(; rfm210.current_id_index_sensor < RFM210_MAX_DEVICES; rfm210.current_id_index_sensor++) {
		if(rfm210.payload_sensor_last_change[rfm210.current_id_index_sensor] != 0) {
			if(rfm210_check_sensor_timeout(rfm210.current_id_index_sensor)) {
				continue;
			}
			response->identifiers_chunk_data[response_index] = rfm210.current_id_index_sensor;
			response_index++;
			if(response_index == 60) {
				break;
			}
		}
	}

	response->identifiers_length = response_index;
	response->identifiers_chunk_offset = rfm210.current_chunk_offset_sensor;

	if(rfm210.current_id_index_sensor == RFM210_MAX_DEVICES) {
		rfm210.current_id_index_sensor = 0;
		rfm210.current_chunk_offset_sensor = 0;
	} else {
		rfm210.current_chunk_offset_sensor += response_index;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_station_data(const GetStationData *data, GetStationData_Response *response) {
	if(rfm210.payload_station_last_change[data->identifier] == 0) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rfm210_check_station_timeout(data->identifier)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(GetStationData_Response);

	RFM210PacketStation packet;
	rfm210_fill_packet_station(&rfm210, data->identifier, &packet);
	response->temperature    = packet.temperature;
	response->humidity       = packet.humidity;
	response->wind_speed     = packet.wind_speed;
	response->gust_speed     = packet.gust_speed;
	response->rain           = packet.rain;
	response->wind_direction = packet.wind_direction;
	response->battery_low    = packet.battery_low;
	response->last_change    = ((uint32_t)(system_timer_get_ms() - rfm210.payload_station_last_change[data->identifier])) / 1000;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_sensor_data(const GetSensorData *data, GetSensorData_Response *response) {
	if(rfm210.payload_sensor_last_change[data->identifier] == 0) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(rfm210_check_sensor_timeout(data->identifier)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(GetSensorData_Response);

	RFM210PacketSensor packet;
	rfm210_fill_packet_sensor(&rfm210, data->identifier, &packet);
	response->temperature    = packet.temperature;
	response->humidity       = packet.humidity;
	response->last_change    = ((uint32_t)(system_timer_get_ms() - rfm210.payload_sensor_last_change[data->identifier])) / 1000;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_station_callback_configuration(const SetStationCallbackConfiguration *data) {
	rfm210.callback_enabled_station = data->enable_callback;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_station_callback_configuration(const GetStationCallbackConfiguration *data, GetStationCallbackConfiguration_Response *response) {
	response->header.length   = sizeof(GetStationCallbackConfiguration_Response);
	response->enable_callback = rfm210.callback_enabled_station;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_sensor_callback_configuration(const SetSensorCallbackConfiguration *data) {
	rfm210.callback_enabled_sensor = data->enable_callback;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_sensor_callback_configuration(const GetSensorCallbackConfiguration *data, GetSensorCallbackConfiguration_Response *response) {
	response->header.length   = sizeof(GetSensorCallbackConfiguration_Response);
	response->enable_callback = rfm210.callback_enabled_sensor;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


bool handle_station_data_callback(void) {
	static bool is_buffered = false;
	static StationData_Callback cb;
	static uint32_t last_change[RFM210_MAX_DEVICES] = {0};

	if(!rfm210.callback_enabled_station) {
		return false;
	}

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(StationData_Callback), FID_CALLBACK_STATION_DATA);
		uint16_t id;
		for(id = 0; id < RFM210_MAX_DEVICES; id++) {
			if(rfm210.payload_station_last_change[id] != 0 && (((uint32_t)(rfm210.payload_station_last_change[id] - last_change[id])) > RFM210_MIN_MESSAGE_GAP_MS)) {
				last_change[id] = rfm210.payload_station_last_change[id];

				RFM210PacketStation packet;
				rfm210_fill_packet_station(&rfm210, id, &packet);
				cb.identifier     = id;
				cb.temperature    = packet.temperature;
				cb.humidity       = packet.humidity;
				cb.wind_speed     = packet.wind_speed;
				cb.gust_speed     = packet.gust_speed;
				cb.rain           = packet.rain;
				cb.wind_direction = packet.wind_direction;
				cb.battery_low    = packet.battery_low;

				break;
			}
		}

		if(id == RFM210_MAX_DEVICES) {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(StationData_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

bool handle_sensor_data_callback(void) {
	static bool is_buffered = false;
	static SensorData_Callback cb;
	static uint32_t last_change[RFM210_MAX_DEVICES] = {0};

	if(!rfm210.callback_enabled_sensor) {
		return false;
	}

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(SensorData_Callback), FID_CALLBACK_SENSOR_DATA);
		uint16_t id;
		for(id = 0; id < RFM210_MAX_DEVICES; id++) {
			if(rfm210.payload_sensor_last_change[id] != 0 && (((uint32_t)(rfm210.payload_sensor_last_change[id] - last_change[id])) > RFM210_MIN_MESSAGE_GAP_MS)) {
				last_change[id] = rfm210.payload_sensor_last_change[id];

				RFM210PacketSensor packet;
				rfm210_fill_packet_sensor(&rfm210, id, &packet);
				cb.identifier   = id;
				cb.temperature  = packet.temperature;
				cb.humidity     = packet.humidity;

				break;
			}
		}

		if(id == RFM210_MAX_DEVICES) {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(SensorData_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	communication_callback_init();
}
