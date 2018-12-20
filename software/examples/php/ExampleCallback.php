<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletOutdoorWeather.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletOutdoorWeather;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Outdoor Weather Bricklet

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

    if ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_N) {
        echo "Wind Direction (Station): N\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NNE) {
        echo "Wind Direction (Station): NNE\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NE) {
        echo "Wind Direction (Station): NE\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_ENE) {
        echo "Wind Direction (Station): ENE\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_E) {
        echo "Wind Direction (Station): E\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_ESE) {
        echo "Wind Direction (Station): ESE\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SE) {
        echo "Wind Direction (Station): SE\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SSE) {
        echo "Wind Direction (Station): SSE\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_S) {
        echo "Wind Direction (Station): S\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SSW) {
        echo "Wind Direction (Station): SSW\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SW) {
        echo "Wind Direction (Station): SW\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_WSW) {
        echo "Wind Direction (Station): WSW\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_W) {
        echo "Wind Direction (Station): W\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_WNW) {
        echo "Wind Direction (Station): WNW\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NW) {
        echo "Wind Direction (Station): NW\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NNW) {
        echo "Wind Direction (Station): NNW\n";
    } elseif ($wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_ERROR) {
        echo "Wind Direction (Station): Error\n";
    }

    echo "Battery Low (Station): $battery_low\n";
    echo "\n";
}

// Callback function for sensor data callback
function cb_sensorData($identifier, $temperature, $humidity)
{
    echo "Identifier (Sensor): $identifier\n";
    echo "Temperature (Sensor): " . $temperature/10.0 . " °C\n";
    echo "Humidity (Sensor): $humidity %RH\n";
    echo "\n";
}

$ipcon = new IPConnection(); // Create IP connection
$ow = new BrickletOutdoorWeather(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Enable station data callbacks
$ow->setStationCallbackConfiguration(TRUE);

// Enable sensor data callbacks
$ow->setSensorCallbackConfiguration(TRUE);

// Register station data callback to function cb_stationData
$ow->registerCallback(BrickletOutdoorWeather::CALLBACK_STATION_DATA, 'cb_stationData');

// Register sensor data callback to function cb_sensorData
$ow->registerCallback(BrickletOutdoorWeather::CALLBACK_SENSOR_DATA, 'cb_sensorData');

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
