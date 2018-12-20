#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletOutdoorWeather;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Outdoor Weather Bricklet

# Callback subroutine for station data callback
sub cb_station_data
{
    my ($identifier, $temperature, $humidity, $wind_speed, $gust_speed, $rain,
        $wind_direction, $battery_low) = @_;

    print "Identifier (Station): $identifier\n";
    print "Temperature (Station): " . $temperature/10.0 . " °C\n";
    print "Humidity (Station): $humidity %RH\n";
    print "Wind Speed (Station): " . $wind_speed/10.0 . " m/s\n";
    print "Gust Speed (Station): " . $gust_speed/10.0 . " m/s\n";
    print "Rain (Station): " . $rain/10.0 . " mm\n";

    if ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_N)
    {
        print "Wind Direction (Station): N\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_NNE)
    {
        print "Wind Direction (Station): NNE\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_NE)
    {
        print "Wind Direction (Station): NE\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_ENE)
    {
        print "Wind Direction (Station): ENE\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_E)
    {
        print "Wind Direction (Station): E\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_ESE)
    {
        print "Wind Direction (Station): ESE\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_SE)
    {
        print "Wind Direction (Station): SE\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_SSE)
    {
        print "Wind Direction (Station): SSE\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_S)
    {
        print "Wind Direction (Station): S\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_SSW)
    {
        print "Wind Direction (Station): SSW\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_SW)
    {
        print "Wind Direction (Station): SW\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_WSW)
    {
        print "Wind Direction (Station): WSW\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_W)
    {
        print "Wind Direction (Station): W\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_WNW)
    {
        print "Wind Direction (Station): WNW\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_NW)
    {
        print "Wind Direction (Station): NW\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_NNW)
    {
        print "Wind Direction (Station): NNW\n";
    }
    elsif ($wind_direction == Tinkerforge::BrickletOutdoorWeather->WIND_DIRECTION_ERROR)
    {
        print "Wind Direction (Station): Error\n";
    }

    print "Battery Low (Station): $battery_low\n";
    print "\n";
}

# Callback subroutine for sensor data callback
sub cb_sensor_data
{
    my ($identifier, $temperature, $humidity) = @_;

    print "Identifier (Sensor): $identifier\n";
    print "Temperature (Sensor): " . $temperature/10.0 . " °C\n";
    print "Humidity (Sensor): $humidity %RH\n";
    print "\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $ow = Tinkerforge::BrickletOutdoorWeather->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Enable station data callbacks
$ow->set_station_callback_configuration(1);

# Enable sensor data callbacks
$ow->set_sensor_callback_configuration(1);

# Register station data callback to subroutine cb_station_data
$ow->register_callback($ow->CALLBACK_STATION_DATA, 'cb_station_data');

# Register sensor data callback to subroutine cb_sensor_data
$ow->register_callback($ow->CALLBACK_SENSOR_DATA, 'cb_sensor_data');

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
