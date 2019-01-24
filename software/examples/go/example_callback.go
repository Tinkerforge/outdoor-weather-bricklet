package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
	"github.com/Tinkerforge/go-api-bindings/outdoor_weather_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Outdoor Weather Bricklet.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	ow, _ := outdoor_weather_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Enable station data callbacks
	ow.SetStationCallbackConfiguration(true)

	// Enable sensor data callbacks
	ow.SetSensorCallbackConfiguration(true)

	ow.RegisterStationDataCallback(func(identifier uint8, temperature int16, humidity uint8, windSpeed uint32, gustSpeed uint32, rain uint32, windDirection outdoor_weather_bricklet.WindDirection, batteryLow bool) {
		fmt.Printf("Identifier (Station): %d\n", identifier)
		fmt.Printf("Temperature (Station): %f °C\n", float64(temperature)/10.0)
		fmt.Printf("Humidity (Station): %d %RH\n", humidity)
		fmt.Printf("Wind Speed (Station): %f m/s\n", float64(windSpeed)/10.0)
		fmt.Printf("Gust Speed (Station): %f m/s\n", float64(gustSpeed)/10.0)
		fmt.Printf("Rain (Station): %f mm\n", float64(rain)/10.0)

		if windDirection == outdoor_weather_bricklet.WindDirectionN {
			fmt.Println("Wind Direction (Station): N")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionNNE {
			fmt.Println("Wind Direction (Station): NNE")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionNE {
			fmt.Println("Wind Direction (Station): NE")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionENE {
			fmt.Println("Wind Direction (Station): ENE")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionE {
			fmt.Println("Wind Direction (Station): E")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionESE {
			fmt.Println("Wind Direction (Station): ESE")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionSE {
			fmt.Println("Wind Direction (Station): SE")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionSSE {
			fmt.Println("Wind Direction (Station): SSE")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionS {
			fmt.Println("Wind Direction (Station): S")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionSSW {
			fmt.Println("Wind Direction (Station): SSW")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionSW {
			fmt.Println("Wind Direction (Station): SW")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionWSW {
			fmt.Println("Wind Direction (Station): WSW")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionW {
			fmt.Println("Wind Direction (Station): W")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionWNW {
			fmt.Println("Wind Direction (Station): WNW")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionNW {
			fmt.Println("Wind Direction (Station): NW")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionNNW {
			fmt.Println("Wind Direction (Station): NNW")
		} else if windDirection == outdoor_weather_bricklet.WindDirectionError {
			fmt.Println("Wind Direction (Station): Error")
		}

		fmt.Printf("Battery Low (Station): %s\n", batteryLow)
		fmt.Println()
	})

	ow.RegisterSensorDataCallback(func(identifier uint8, temperature int16, humidity uint8) {
		fmt.Printf("Identifier (Sensor): %d\n", identifier)
		fmt.Printf("Temperature (Sensor): %f °C\n", float64(temperature)/10.0)
		fmt.Printf("Humidity (Sensor): %d %RH\n", humidity)
		fmt.Println()
	})

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
