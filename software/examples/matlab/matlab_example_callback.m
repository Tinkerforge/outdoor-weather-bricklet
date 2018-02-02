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
    fprintf('Wind Direction (Station): %i\n', e.windDirection);
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
