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
		case FID_GET_WEATHER_STATION_IDENTIFIERS_LOW_LEVEL: return get_weather_station_identifiers_low_level(message, response);
		case FID_GET_WEATHER_STATION_DATA: return get_weather_station_data(message, response);
		case FID_SET_WEATHER_STATION_CALLBACK_CONFIGURATION: return set_weather_station_callback_configuration(message);
		case FID_GET_WEATHER_STATION_CALLBACK_CONFIGURATION: return get_weather_station_callback_configuration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse get_weather_station_identifiers_low_level(const GetWeatherStationIdentifiersLowLevel *data, GetWeatherStationIdentifiersLowLevel_Response *response) {
	response->header.length = sizeof(GetWeatherStationIdentifiersLowLevel_Response);

	uint8_t response_index = 0;
	for(; rfm210.current_id_index < RFM210_MAX_DEVICES; rfm210.current_id_index++) {
		if(rfm210.payload_last_change[rfm210.current_id_index] != 0) {
			response->identifiers_chunk_data[response_index] = rfm210.current_id_index;
			response_index++;
			if(response_index == 60) {
				break;
			}
		}
	}

	response->identifiers_length = response_index;
	response->identifiers_chunk_offset = rfm210.current_chunk_offset;

	if(rfm210.current_id_index == RFM210_MAX_DEVICES) {
		rfm210.current_id_index = 0;
		rfm210.current_chunk_offset = 0;
	} else {
		rfm210.current_chunk_offset += response_index;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_weather_station_data(const GetWeatherStationData *data, GetWeatherStationData_Response *response) {
	if(rfm210.payload_last_change[data->identifier] == 0) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(GetWeatherStationData_Response);

	RFM210Packet packet;
	rfm210_fill_packet(&rfm210, data->identifier, &packet);
	response->temperature    = packet.temperature;
	response->humidity       = packet.humidity;
	response->wind_speed     = packet.wind_speed;
	response->gust_speed     = packet.gust_speed;
	response->rain           = packet.rain;
	response->wind_direction = packet.wind_direction;
	response->battery_low    = packet.battery_low;
	response->last_change    = ((uint32_t)(system_timer_get_ms() - rfm210.payload_last_change[data->identifier])) / 1000;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_weather_station_callback_configuration(const SetWeatherStationCallbackConfiguration *data) {
	rfm210.callback_enabled = data->enable_callback;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_weather_station_callback_configuration(const GetWeatherStationCallbackConfiguration *data, GetWeatherStationCallbackConfiguration_Response *response) {
	response->header.length   = sizeof(GetWeatherStationCallbackConfiguration_Response);
	response->enable_callback = rfm210.callback_enabled;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}




bool handle_weather_station_data_callback(void) {
	static bool is_buffered = false;
	static WeatherStationData_Callback cb;
	static uint32_t last_change[RFM210_MAX_DEVICES] = {0};

	if(!rfm210.callback_enabled) {
		return false;
	}

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(WeatherStationData_Callback), FID_CALLBACK_WEATHER_STATION_DATA);
		uint16_t id;
		for(id = 0; id < RFM210_MAX_DEVICES; id++) {
			if(rfm210.payload_last_change[id] != 0 && (((uint32_t)(rfm210.payload_last_change[id] - last_change[id])) > 4)) {
				last_change[id] = rfm210.payload_last_change[id];

				RFM210Packet packet;
				rfm210_fill_packet(&rfm210, id, &packet);
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
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(WeatherStationData_Callback));
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
