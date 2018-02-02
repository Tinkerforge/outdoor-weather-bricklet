<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletOutdoorWeather.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletOutdoorWeather;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Outdoor Weather Bricklet

// Callback function for sensor data callback
function cb_sensorData($identifier, $temperature, $humidity)
{
    echo "Identifier (Sensor): $identifier\n";
    echo "Temperature (Sensor): " . $temperature/10.0 . " °C\n";
    echo "Humidity (Sensor): $humidity %RH\n";
    echo "\n";
}

// Callback function for station data callback
function cb_stationData($identifier, $temperature, $humidity, $wind_speed, $gust_speed,
                        $rain, $wind_direction, $battery_low)
{
    echo "Identifier (Station): $identifier\n";
    echo "Temperature (Station): " . $temperature/10.0 . " °C\n";
    echo "Humidity (Station): $humidity %RH\n";
    echo "Wind Speed (Station): " . $wind_speed/10.0 . " m/s\n";
    echo "Gust Speed (Station): " . $gust_speed/10.0 . " m/s\n";
    echo "Rain (Station): " . $rain/10.0 . " mm\n";
    echo "Wind Direction (Station): $wind_direction\n";
    echo "Battery Low (Station): $battery_low\n";
    echo "\n";
}

$ipcon = new IPConnection(); // Create IP connection
$ow = new BrickletOutdoorWeather(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Enable sensor data callbacks
$ow->setSensorCallbackConfiguration(TRUE);

// Enable station data callbacks
$ow->setStationCallbackConfiguration(TRUE);

// Register sensor data callback to function cb_sensorData
$ow->registerCallback(BrickletOutdoorWeather::CALLBACK_SENSOR_DATA, 'cb_sensorData');

// Register station data callback to function cb_stationData
$ow->registerCallback(BrickletOutdoorWeather::CALLBACK_STATION_DATA, 'cb_stationData');

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
