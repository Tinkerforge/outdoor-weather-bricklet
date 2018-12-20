var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your Outdoor Weather Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var ow = new Tinkerforge.BrickletOutdoorWeather(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Enable station data callbacks
        ow.setStationCallbackConfiguration(true);

        // Enable sensor data callbacks
        ow.setSensorCallbackConfiguration(true);
    }
);

// Register station data callback
ow.on(Tinkerforge.BrickletOutdoorWeather.CALLBACK_STATION_DATA,
    // Callback function for station data callback
    function (identifier, temperature, humidity, windSpeed, gustSpeed, rain,
              windDirection, batteryLow) {
        console.log('Identifier (Station): ' + identifier);
        console.log('Temperature (Station): ' + temperature/10.0 + ' °C');
        console.log('Humidity (Station): ' + humidity + ' %RH');
        console.log('Wind Speed (Station): ' + windSpeed/10.0 + ' m/s');
        console.log('Gust Speed (Station): ' + gustSpeed/10.0 + ' m/s');
        console.log('Rain (Station): ' + rain/10.0 + ' mm');

        if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_N) {
            console.log('Wind Direction (Station): N');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NNE) {
            console.log('Wind Direction (Station): NNE');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NE) {
            console.log('Wind Direction (Station): NE');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_ENE) {
            console.log('Wind Direction (Station): ENE');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_E) {
            console.log('Wind Direction (Station): E');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_ESE) {
            console.log('Wind Direction (Station): ESE');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SE) {
            console.log('Wind Direction (Station): SE');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SSE) {
            console.log('Wind Direction (Station): SSE');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_S) {
            console.log('Wind Direction (Station): S');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SSW) {
            console.log('Wind Direction (Station): SSW');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_SW) {
            console.log('Wind Direction (Station): SW');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_WSW) {
            console.log('Wind Direction (Station): WSW');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_W) {
            console.log('Wind Direction (Station): W');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_WNW) {
            console.log('Wind Direction (Station): WNW');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NW) {
            console.log('Wind Direction (Station): NW');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_NNW) {
            console.log('Wind Direction (Station): NNW');
        }
        else if(windDirection === Tinkerforge.BrickletOutdoorWeather.WIND_DIRECTION_ERROR) {
            console.log('Wind Direction (Station): Error');
        }

        console.log('Battery Low (Station): ' + batteryLow);
        console.log();
    }
);

// Register sensor data callback
ow.on(Tinkerforge.BrickletOutdoorWeather.CALLBACK_SENSOR_DATA,
    // Callback function for sensor data callback
    function (identifier, temperature, humidity) {
        console.log('Identifier (Sensor): ' + identifier);
        console.log('Temperature (Sensor): ' + temperature/10.0 + ' °C');
        console.log('Humidity (Sensor): ' + humidity + ' %RH');
        console.log();
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
