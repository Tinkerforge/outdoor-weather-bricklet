using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Outdoor Weather Bricklet

	// Callback function for station data callback
	static void StationDataCB(BrickletOutdoorWeather sender, byte identifier,
	                          short temperature, byte humidity, long windSpeed,
	                          long gustSpeed, long rain, byte windDirection,
	                          bool batteryLow)
	{
		Console.WriteLine("Identifier (Station): " + identifier);
		Console.WriteLine("Temperature (Station): " + temperature/10.0 + " °C");
		Console.WriteLine("Humidity (Station): " + humidity + " %RH");
		Console.WriteLine("Wind Speed (Station): " + windSpeed/10.0 + " m/s");
		Console.WriteLine("Gust Speed (Station): " + gustSpeed/10.0 + " m/s");
		Console.WriteLine("Rain (Station): " + rain/10.0 + " mm");
		Console.WriteLine("Wind Direction (Station): " + windDirection);
		Console.WriteLine("Battery Low (Station): " + batteryLow);
		Console.WriteLine("");
	}

	// Callback function for sensor data callback
	static void SensorDataCB(BrickletOutdoorWeather sender, byte identifier,
	                         short temperature, byte humidity)
	{
		Console.WriteLine("Identifier (Sensor): " + identifier);
		Console.WriteLine("Temperature (Sensor): " + temperature/10.0 + " °C");
		Console.WriteLine("Humidity (Sensor): " + humidity + " %RH");
		Console.WriteLine("");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletOutdoorWeather ow = new BrickletOutdoorWeather(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Enable station data callbacks
		ow.SetStationCallbackConfiguration(true);

		// Enable sensor data callbacks
		ow.SetSensorCallbackConfiguration(true);

		// Register station data callback to function StationDataCB
		ow.StationDataCallback += StationDataCB;

		// Register sensor data callback to function SensorDataCB
		ow.SensorDataCallback += SensorDataCB;

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
