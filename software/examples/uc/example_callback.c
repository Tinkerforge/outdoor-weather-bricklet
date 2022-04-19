// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_outdoor_weather.h"

#define UID "XYZ" // Change XYZ to the UID of your Outdoor Weather Bricklet

void check(int rc, const char* msg);

void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);


// Callback function for station data callback
static void station_data_handler(TF_OutdoorWeather *device, uint8_t identifier,
                                 int16_t temperature, uint8_t humidity,
                                 uint32_t wind_speed, uint32_t gust_speed, uint32_t rain,
                                 uint8_t wind_direction, bool battery_low,
                                 void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Identifier (Station): %I8u\n", identifier);
	tf_hal_printf("Temperature (Station): %d 1/%d °C\n", temperature, 10);
	tf_hal_printf("Humidity (Station): %I8u %%RH\n", humidity);
	tf_hal_printf("Wind Speed (Station): %d 1/%d m/s\n", wind_speed, 10);
	tf_hal_printf("Gust Speed (Station): %d 1/%d m/s\n", gust_speed, 10);
	tf_hal_printf("Rain (Station): %d 1/%d mm\n", rain, 10);

	if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_N) {
		tf_hal_printf("Wind Direction (Station): N\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_NNE) {
		tf_hal_printf("Wind Direction (Station): NNE\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_NE) {
		tf_hal_printf("Wind Direction (Station): NE\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_ENE) {
		tf_hal_printf("Wind Direction (Station): ENE\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_E) {
		tf_hal_printf("Wind Direction (Station): E\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_ESE) {
		tf_hal_printf("Wind Direction (Station): ESE\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_SE) {
		tf_hal_printf("Wind Direction (Station): SE\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_SSE) {
		tf_hal_printf("Wind Direction (Station): SSE\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_S) {
		tf_hal_printf("Wind Direction (Station): S\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_SSW) {
		tf_hal_printf("Wind Direction (Station): SSW\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_SW) {
		tf_hal_printf("Wind Direction (Station): SW\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_WSW) {
		tf_hal_printf("Wind Direction (Station): WSW\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_W) {
		tf_hal_printf("Wind Direction (Station): W\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_WNW) {
		tf_hal_printf("Wind Direction (Station): WNW\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_NW) {
		tf_hal_printf("Wind Direction (Station): NW\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_NNW) {
		tf_hal_printf("Wind Direction (Station): NNW\n");
	} else if (wind_direction == TF_OUTDOOR_WEATHER_WIND_DIRECTION_ERROR) {
		tf_hal_printf("Wind Direction (Station): Error\n");
	}

	tf_hal_printf("Battery Low (Station): %s\n", battery_low ? "true" : "false");
	tf_hal_printf("\n");
}

// Callback function for sensor data callback
static void sensor_data_handler(TF_OutdoorWeather *device, uint8_t identifier,
                                int16_t temperature, uint8_t humidity, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Identifier (Sensor): %I8u\n", identifier);
	tf_hal_printf("Temperature (Sensor): %d 1/%d °C\n", temperature, 10);
	tf_hal_printf("Humidity (Sensor): %I8u %%RH\n", humidity);
	tf_hal_printf("\n");
}

static TF_OutdoorWeather ow;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_outdoor_weather_create(&ow, UID, hal), "create device object");

	// Enable station data callbacks
	check(tf_outdoor_weather_set_station_callback_configuration(&ow,
	                                                            true), "call set_station_callback_configuration");

	// Enable sensor data callbacks
	check(tf_outdoor_weather_set_sensor_callback_configuration(&ow,
	                                                           true), "call set_sensor_callback_configuration");

	// Register station data callback to function station_data_handler
	tf_outdoor_weather_register_station_data_callback(&ow,
	                                                  station_data_handler,
	                                                  NULL);

	// Register sensor data callback to function sensor_data_handler
	tf_outdoor_weather_register_sensor_data_callback(&ow,
	                                                 sensor_data_handler,
	                                                 NULL);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
