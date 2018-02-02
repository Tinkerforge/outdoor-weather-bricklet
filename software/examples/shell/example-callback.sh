#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Outdoor Weather Bricklet

# Enable station data callbacks
tinkerforge call outdoor-weather-bricklet $uid set-station-callback-configuration true

# Enable sensor data callbacks
tinkerforge call outdoor-weather-bricklet $uid set-sensor-callback-configuration true

# Handle incoming station data callbacks
tinkerforge dispatch outdoor-weather-bricklet $uid station-data &

# Handle incoming sensor data callbacks
tinkerforge dispatch outdoor-weather-bricklet $uid sensor-data &

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
