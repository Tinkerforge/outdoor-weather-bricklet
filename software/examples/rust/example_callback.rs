use std::{error::Error, io, thread};
use tinkerforge::{ip_connection::IpConnection, outdoor_weather_bricklet::*};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Outdoor Weather Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let ow = OutdoorWeatherBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Enable station data callbacks
    ow.set_station_callback_configuration(true);

    // Enable sensor data callbacks
    ow.set_sensor_callback_configuration(true);

    let station_data_receiver = ow.get_station_data_callback_receiver();

    // Spawn thread to handle received callback messages.
    // This thread ends when the `ow` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for station_data in station_data_receiver {
            println!("Identifier (Station): {}", station_data.identifier);
            println!(
                "Temperature (Station): {} °C",
                station_data.temperature as f32 / 10.0
            );
            println!("Humidity (Station): {} %RH", station_data.humidity);
            println!(
                "Wind Speed (Station): {} m/s",
                station_data.wind_speed as f32 / 10.0
            );
            println!(
                "Gust Speed (Station): {} m/s",
                station_data.gust_speed as f32 / 10.0
            );
            println!("Rain (Station): {} mm", station_data.rain as f32 / 10.0);

            if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_N {
                println!("Wind Direction (Station): N");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_NNE {
                println!("Wind Direction (Station): NNE");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_NE {
                println!("Wind Direction (Station): NE");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_ENE {
                println!("Wind Direction (Station): ENE");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_E {
                println!("Wind Direction (Station): E");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_ESE {
                println!("Wind Direction (Station): ESE");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_SE {
                println!("Wind Direction (Station): SE");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_SSE {
                println!("Wind Direction (Station): SSE");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_S {
                println!("Wind Direction (Station): S");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_SSW {
                println!("Wind Direction (Station): SSW");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_SW {
                println!("Wind Direction (Station): SW");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_WSW {
                println!("Wind Direction (Station): WSW");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_W {
                println!("Wind Direction (Station): W");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_WNW {
                println!("Wind Direction (Station): WNW");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_NW {
                println!("Wind Direction (Station): NW");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_NNW {
                println!("Wind Direction (Station): NNW");
            } else if station_data.wind_direction == OUTDOOR_WEATHER_BRICKLET_WIND_DIRECTION_ERROR {
                println!("Wind Direction (Station): Error");
            }

            println!("Battery Low (Station): {}", station_data.battery_low);
            println!();
        }
    });

    let sensor_data_receiver = ow.get_sensor_data_callback_receiver();

    // Spawn thread to handle received callback messages.
    // This thread ends when the `ow` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for sensor_data in sensor_data_receiver {
            println!("Identifier (Sensor): {}", sensor_data.identifier);
            println!(
                "Temperature (Sensor): {} °C",
                sensor_data.temperature as f32 / 10.0
            );
            println!("Humidity (Sensor): {} %RH", sensor_data.humidity);
            println!();
        }
    });

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
