use std::{error::Error, io, thread};
use tinkerforge::{ipconnection::IpConnection, outdoor_weather_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Outdoor Weather Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let outdoor_weather_bricklet = OutdoorWeatherBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Enable station data callbacks
    outdoor_weather_bricklet.set_station_callback_configuration(true);

    // Enable sensor data callbacks
    outdoor_weather_bricklet.set_sensor_callback_configuration(true);

    //Create listener for station data events.
    let station_data_listener = outdoor_weather_bricklet.get_station_data_receiver();
    // Spawn thread to handle received events. This thread ends when the outdoor_weather_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in station_data_listener {
            println!("Identifier (Station): {}", event.identifier);
            println!("Temperature (Station): {}{}", event.temperature as f32 / 10.0, " °C");
            println!("Humidity (Station): {}{}", event.humidity, " %RH");
            println!("Wind Speed (Station): {}{}", event.wind_speed as f32 / 10.0, " m/s");
            println!("Gust Speed (Station): {}{}", event.gust_speed as f32 / 10.0, " m/s");
            println!("Rain (Station): {}{}", event.rain as f32 / 10.0, " mm");
            println!("Wind Direction (Station): {}", event.wind_direction);
            println!("Battery Low (Station): {}", event.battery_low);
            println!();
        }
    });

    //Create listener for sensor data events.
    let sensor_data_listener = outdoor_weather_bricklet.get_sensor_data_receiver();
    // Spawn thread to handle received events. This thread ends when the outdoor_weather_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in sensor_data_listener {
            println!("Identifier (Sensor): {}", event.identifier);
            println!("Temperature (Sensor): {}{}", event.temperature as f32 / 10.0, " °C");
            println!("Humidity (Sensor): {}{}", event.humidity, " %RH");
            println!();
        }
    });

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
