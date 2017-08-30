#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "abc3"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_outdoor_weather import BrickletOutdoorWeather

def cb_weather_station_data(identifier, temperature, humidity, wind_speed, gust_speed, rain, wind_direction, battery_low):
    print("Weather Station Data ({0}):".format(identifier))
    print(" temperature: {0}".format(temperature))
    print(" humidity: {0}".format(humidity))
    print(" wind_speed: {0}".format(wind_speed))
    print(" gust_speed: {0}".format(gust_speed))
    print(" rain: {0}".format(rain))
    print(" wind_direction: {0}".format(wind_direction))
    print(" battery_low: {0}".format(battery_low))
    print("")


if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    ow = BrickletOutdoorWeather(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected


    ow.register_callback(ow.CALLBACK_WEATHER_STATION_DATA, cb_weather_station_data)
    ow.set_weather_station_callback_configuration(True)


    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
