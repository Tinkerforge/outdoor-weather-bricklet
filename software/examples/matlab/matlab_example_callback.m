function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletOutdoorWeather;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Outdoor Weather Bricklet

    ipcon = IPConnection(); % Create IP connection
    ow = handle(BrickletOutdoorWeather(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Enable station data callbacks
    ow.setStationCallbackConfiguration(true);

    % Enable sensor data callbacks
    ow.setSensorCallbackConfiguration(true);

    % Register station data callback to function cb_station_data
    set(ow, 'StationDataCallback', @(h, e) cb_station_data(e));

    % Register sensor data callback to function cb_sensor_data
    set(ow, 'SensorDataCallback', @(h, e) cb_sensor_data(e));

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for station data callback
function cb_station_data(e)
    fprintf('Identifier (Station): %i\n', e.identifier);
    fprintf('Temperature (Station): %g °C\n', e.temperature/10.0);
    fprintf('Humidity (Station): %i %%RH\n', e.humidity);
    fprintf('Wind Speed (Station): %g m/s\n', e.windSpeed/10.0);
    fprintf('Gust Speed (Station): %g m/s\n', e.gustSpeed/10.0);
    fprintf('Rain (Station): %g mm\n', e.rain/10.0);

    if e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_N
        fprintf('Wind Direction (Station): N\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NNE
        fprintf('Wind Direction (Station): NNE\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NE
        fprintf('Wind Direction (Station): NE\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_ENE
        fprintf('Wind Direction (Station): ENE\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_E
        fprintf('Wind Direction (Station): E\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_ESE
        fprintf('Wind Direction (Station): ESE\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SE
        fprintf('Wind Direction (Station): SE\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SSE
        fprintf('Wind Direction (Station): SSE\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_S
        fprintf('Wind Direction (Station): S\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SSW
        fprintf('Wind Direction (Station): SSW\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SW
        fprintf('Wind Direction (Station): SW\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_WSW
        fprintf('Wind Direction (Station): WSW\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_W
        fprintf('Wind Direction (Station): W\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_WNW
        fprintf('Wind Direction (Station): WNW\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NW
        fprintf('Wind Direction (Station): NW\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NNW
        fprintf('Wind Direction (Station): NNW\n');
    elseif e.windDirection == com.tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_ERROR
        fprintf('Wind Direction (Station): Error\n');
    end

    fprintf('Battery Low (Station): %i\n', e.batteryLow);
    fprintf('\n');
end

% Callback function for sensor data callback
function cb_sensor_data(e)
    fprintf('Identifier (Sensor): %i\n', e.identifier);
    fprintf('Temperature (Sensor): %g °C\n', e.temperature/10.0);
    fprintf('Humidity (Sensor): %i %%RH\n', e.humidity);
    fprintf('\n');
end
