function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Outdoor Weather Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    ow = javaObject("com.tinkerforge.BrickletOutdoorWeather", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Enable sensor data callbacks
    ow.setSensorCallbackConfiguration(true);

    % Enable station data callbacks
    ow.setStationCallbackConfiguration(true);

    % Register sensor data callback to function cb_sensor_data
    ow.addSensorDataCallback(@cb_sensor_data);

    % Register station data callback to function cb_station_data
    ow.addStationDataCallback(@cb_station_data);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for sensor data callback
function cb_sensor_data(e)
    fprintf("Identifier (Sensor): %d\n", e.identifier);
    fprintf("Temperature (Sensor): %g °C\n", e.temperature/10.0);
    fprintf("Humidity (Sensor): %d %%RH\n", e.humidity);
    fprintf("\n");
end

% Callback function for station data callback
function cb_station_data(e)
    fprintf("Identifier (Station): %d\n", e.identifier);
    fprintf("Temperature (Station): %g °C\n", e.temperature/10.0);
    fprintf("Humidity (Station): %d %%RH\n", e.humidity);
    fprintf("Wind Speed (Station): %g m/s\n", java2int(e.windSpeed)/10.0);
    fprintf("Gust Speed (Station): %g m/s\n", java2int(e.gustSpeed)/10.0);
    fprintf("Rain (Station): %g mm\n", java2int(e.rain)/10.0);
    fprintf("Wind Direction (Station): %d\n", e.windDirection);
    fprintf("Battery Low (Station): %d\n", e.batteryLow);
    fprintf("\n");
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
