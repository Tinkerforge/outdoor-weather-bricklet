#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "abc3"

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_outdoor_weather import BrickletOutdoorWeather

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    ow = BrickletOutdoorWeather(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    identifiers = ow.get_weather_station_identifiers()
    for identifier in identifiers:
        print(identifier, ow.get_weather_station_data(identifier))

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
