#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_outdoor_weather'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Outdoor Weather Bricklet

ipcon = IPConnection.new # Create IP connection
ow = BrickletOutdoorWeather.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Enable station data callbacks
ow.set_station_callback_configuration true

# Enable sensor data callbacks
ow.set_sensor_callback_configuration true

# Register station data callback
ow.register_callback(BrickletOutdoorWeather::CALLBACK_STATION_DATA) do |identifier,
                                                                        temperature,
                                                                        humidity,
                                                                        wind_speed,
                                                                        gust_speed, rain,
                                                                        wind_direction,
                                                                        battery_low|
  puts "Identifier (Station): #{identifier}"
  puts "Temperature (Station): #{temperature/10.0} °C"
  puts "Humidity (Station): #{humidity} %RH"
  puts "Wind Speed (Station): #{wind_speed/10.0} m/s"
  puts "Gust Speed (Station): #{gust_speed/10.0} m/s"
  puts "Rain (Station): #{rain/10.0} mm"

  if wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_N
    puts "Wind Direction (Station): N"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NNE
    puts "Wind Direction (Station): NNE"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NE
    puts "Wind Direction (Station): NE"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_ENE
    puts "Wind Direction (Station): ENE"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_E
    puts "Wind Direction (Station): E"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_ESE
    puts "Wind Direction (Station): ESE"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SE
    puts "Wind Direction (Station): SE"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SSE
    puts "Wind Direction (Station): SSE"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_S
    puts "Wind Direction (Station): S"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SSW
    puts "Wind Direction (Station): SSW"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_SW
    puts "Wind Direction (Station): SW"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_WSW
    puts "Wind Direction (Station): WSW"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_W
    puts "Wind Direction (Station): W"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_WNW
    puts "Wind Direction (Station): WNW"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NW
    puts "Wind Direction (Station): NW"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_NNW
    puts "Wind Direction (Station): NNW"
  elsif wind_direction == BrickletOutdoorWeather::WIND_DIRECTION_ERROR
    puts "Wind Direction (Station): Error"
  end

  puts "Battery Low (Station): #{battery_low}"
  puts ''
end

# Register sensor data callback
ow.register_callback(BrickletOutdoorWeather::CALLBACK_SENSOR_DATA) do |identifier,
                                                                       temperature,
                                                                       humidity|
  puts "Identifier (Sensor): #{identifier}"
  puts "Temperature (Sensor): #{temperature/10.0} °C"
  puts "Humidity (Sensor): #{humidity} %RH"
  puts ''
end

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
