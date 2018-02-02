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
        console.log('Wind Direction (Station): ' + windDirection);
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
