#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_outdoor_weather.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your Outdoor Weather Bricklet

// Callback function for sensor data callback
void cb_sensor_data(uint8_t identifier, int16_t temperature, uint8_t humidity,
                    void *user_data) {
	(void)user_data; // avoid unused parameter warning

	printf("Identifier (Sensor): %u\n", identifier);
	printf("Temperature (Sensor): %f °C\n", temperature/10.0);
	printf("Humidity (Sensor): %u %%RH\n", humidity);
	printf("\n");
}

// Callback function for station data callback
void cb_station_data(uint8_t identifier, int16_t temperature, uint8_t humidity,
                     uint32_t wind_speed, uint32_t gust_speed, uint32_t rain,
                     uint8_t wind_direction, bool battery_low, void *user_data) {
	(void)user_data; // avoid unused parameter warning

	printf("Identifier (Station): %u\n", identifier);
	printf("Temperature (Station): %f °C\n", temperature/10.0);
	printf("Humidity (Station): %u %%RH\n", humidity);
	printf("Wind Speed (Station): %f m/s\n", wind_speed/10.0);
	printf("Gust Speed (Station): %f m/s\n", gust_speed/10.0);
	printf("Rain (Station): %f mm\n", rain/10.0);
	printf("Wind Direction (Station): %u\n", wind_direction);
	printf("Battery Low (Station): %s\n", battery_low ? "true" : "false");
	printf("\n");
}

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	OutdoorWeather ow;
	outdoor_weather_create(&ow, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Enable sensor data callbacks
	outdoor_weather_set_sensor_callback_configuration(&ow, true);

	// Enable station data callbacks
	outdoor_weather_set_station_callback_configuration(&ow, true);

	// Register sensor data callback to function cb_sensor_data
	outdoor_weather_register_callback(&ow,
	                                  OUTDOOR_WEATHER_CALLBACK_SENSOR_DATA,
	                                  (void *)cb_sensor_data,
	                                  NULL);

	// Register station data callback to function cb_station_data
	outdoor_weather_register_callback(&ow,
	                                  OUTDOOR_WEATHER_CALLBACK_STATION_DATA,
	                                  (void *)cb_station_data,
	                                  NULL);

	printf("Press key to exit\n");
	getchar();
	outdoor_weather_destroy(&ow);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
