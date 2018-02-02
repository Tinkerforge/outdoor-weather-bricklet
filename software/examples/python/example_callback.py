#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your Outdoor Weather Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_outdoor_weather import BrickletOutdoorWeather

# Callback function for station data callback
def cb_station_data(identifier, temperature, humidity, wind_speed, gust_speed, rain,
                    wind_direction, battery_low):
    print("Identifier (Station): " + str(identifier))
    print("Temperature (Station): " + str(temperature/10.0) + " °C")
    print("Humidity (Station): " + str(humidity) + " %RH")
    print("Wind Speed (Station): " + str(wind_speed/10.0) + " m/s")
    print("Gust Speed (Station): " + str(gust_speed/10.0) + " m/s")
    print("Rain (Station): " + str(rain/10.0) + " mm")
    print("Wind Direction (Station): " + str(wind_direction))
    print("Battery Low (Station): " + str(battery_low))
    print("")

# Callback function for sensor data callback
def cb_sensor_data(identifier, temperature, humidity):
    print("Identifier (Sensor): " + str(identifier))
    print("Temperature (Sensor): " + str(temperature/10.0) + " °C")
    print("Humidity (Sensor): " + str(humidity) + " %RH")
    print("")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    ow = BrickletOutdoorWeather(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Enable station data callbacks
    ow.set_station_callback_configuration(True)

    # Enable sensor data callbacks
    ow.set_sensor_callback_configuration(True)

    # Register station data callback to function cb_station_data
    ow.register_callback(ow.CALLBACK_STATION_DATA, cb_station_data)

    # Register sensor data callback to function cb_sensor_data
    ow.register_callback(ow.CALLBACK_SENSOR_DATA, cb_sensor_data)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
