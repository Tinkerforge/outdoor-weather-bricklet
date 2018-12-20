import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletOutdoorWeather;

public class ExampleCallback {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your Outdoor Weather Bricklet
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletOutdoorWeather ow = new BrickletOutdoorWeather(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Enable station data callbacks
		ow.setStationCallbackConfiguration(true);

		// Enable sensor data callbacks
		ow.setSensorCallbackConfiguration(true);

		// Add station data listener
		ow.addStationDataListener(new BrickletOutdoorWeather.StationDataListener() {
			public void stationData(int identifier, int temperature, int humidity,
			                        long windSpeed, long gustSpeed, long rain,
			                        int windDirection, boolean batteryLow) {
				System.out.println("Identifier (Station): " + identifier);
				System.out.println("Temperature (Station): " + temperature/10.0 + " °C");
				System.out.println("Humidity (Station): " + humidity + " %RH");
				System.out.println("Wind Speed (Station): " + windSpeed/10.0 + " m/s");
				System.out.println("Gust Speed (Station): " + gustSpeed/10.0 + " m/s");
				System.out.println("Rain (Station): " + rain/10.0 + " mm");

				if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_N) {
					System.out.println("Wind Direction (Station): N");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_NNE) {
					System.out.println("Wind Direction (Station): NNE");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_NE) {
					System.out.println("Wind Direction (Station): NE");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_ENE) {
					System.out.println("Wind Direction (Station): ENE");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_E) {
					System.out.println("Wind Direction (Station): E");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_ESE) {
					System.out.println("Wind Direction (Station): ESE");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_SE) {
					System.out.println("Wind Direction (Station): SE");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_SSE) {
					System.out.println("Wind Direction (Station): SSE");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_S) {
					System.out.println("Wind Direction (Station): S");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_SSW) {
					System.out.println("Wind Direction (Station): SSW");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_SW) {
					System.out.println("Wind Direction (Station): SW");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_WSW) {
					System.out.println("Wind Direction (Station): WSW");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_W) {
					System.out.println("Wind Direction (Station): W");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_WNW) {
					System.out.println("Wind Direction (Station): WNW");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_NW) {
					System.out.println("Wind Direction (Station): NW");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_NNW) {
					System.out.println("Wind Direction (Station): NNW");
				} else if(windDirection == BrickletOutdoorWeather.WIND_DIRECTION_ERROR) {
					System.out.println("Wind Direction (Station): Error");
				}

				System.out.println("Battery Low (Station): " + batteryLow);
				System.out.println("");
			}
		});

		// Add sensor data listener
		ow.addSensorDataListener(new BrickletOutdoorWeather.SensorDataListener() {
			public void sensorData(int identifier, int temperature, int humidity) {
				System.out.println("Identifier (Sensor): " + identifier);
				System.out.println("Temperature (Sensor): " + temperature/10.0 + " °C");
				System.out.println("Humidity (Sensor): " + humidity + " %RH");
				System.out.println("");
			}
		});

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
