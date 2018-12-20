function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Outdoor Weather Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    ow = javaObject("com.tinkerforge.BrickletOutdoorWeather", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Enable station data callbacks
    ow.setStationCallbackConfiguration(true);

    % Enable sensor data callbacks
    ow.setSensorCallbackConfiguration(true);

    % Register station data callback to function cb_station_data
    ow.addStationDataCallback(@cb_station_data);

    % Register sensor data callback to function cb_sensor_data
    ow.addSensorDataCallback(@cb_sensor_data);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for station data callback
function cb_station_data(e)
    fprintf("Identifier (Station): %d\n", e.identifier);
    fprintf("Temperature (Station): %g °C\n", e.temperature/10.0);
    fprintf("Humidity (Station): %d %%RH\n", e.humidity);
    fprintf("Wind Speed (Station): %g m/s\n", java2int(e.windSpeed)/10.0);
    fprintf("Gust Speed (Station): %g m/s\n", java2int(e.gustSpeed)/10.0);
    fprintf("Rain (Station): %g mm\n", java2int(e.rain)/10.0);

    if e.windDirection == 0
        fprintf("Wind Direction (Station): N\n");
    elseif e.windDirection == 1
        fprintf("Wind Direction (Station): NNE\n");
    elseif e.windDirection == 2
        fprintf("Wind Direction (Station): NE\n");
    elseif e.windDirection == 3
        fprintf("Wind Direction (Station): ENE\n");
    elseif e.windDirection == 4
        fprintf("Wind Direction (Station): E\n");
    elseif e.windDirection == 5
        fprintf("Wind Direction (Station): ESE\n");
    elseif e.windDirection == 6
        fprintf("Wind Direction (Station): SE\n");
    elseif e.windDirection == 7
        fprintf("Wind Direction (Station): SSE\n");
    elseif e.windDirection == 8
        fprintf("Wind Direction (Station): S\n");
    elseif e.windDirection == 9
        fprintf("Wind Direction (Station): SSW\n");
    elseif e.windDirection == 10
        fprintf("Wind Direction (Station): SW\n");
    elseif e.windDirection == 11
        fprintf("Wind Direction (Station): WSW\n");
    elseif e.windDirection == 12
        fprintf("Wind Direction (Station): W\n");
    elseif e.windDirection == 13
        fprintf("Wind Direction (Station): WNW\n");
    elseif e.windDirection == 14
        fprintf("Wind Direction (Station): NW\n");
    elseif e.windDirection == 15
        fprintf("Wind Direction (Station): NNW\n");
    elseif e.windDirection == 255
        fprintf("Wind Direction (Station): Error\n");
    end

    fprintf("Battery Low (Station): %d\n", e.batteryLow);
    fprintf("\n");
end

% Callback function for sensor data callback
function cb_sensor_data(e)
    fprintf("Identifier (Sensor): %d\n", e.identifier);
    fprintf("Temperature (Sensor): %g °C\n", e.temperature/10.0);
    fprintf("Humidity (Sensor): %d %%RH\n", e.humidity);
    fprintf("\n");
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
