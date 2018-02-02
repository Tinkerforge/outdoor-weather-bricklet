Imports System
Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Outdoor Weather Bricklet

    ' Callback subroutine for station data callback
    Sub StationDataCB(ByVal sender As BrickletOutdoorWeather, ByVal identifier As Byte, _
                      ByVal temperature As Short, ByVal humidity As Byte, _
                      ByVal windSpeed As Long, ByVal gustSpeed As Long, _
                      ByVal rain As Long, ByVal windDirection As Byte, _
                      ByVal batteryLow As Boolean)
        Console.WriteLine("Identifier (Station): " + identifier.ToString())
        Console.WriteLine("Temperature (Station): " + (temperature/10.0).ToString() + " °C")
        Console.WriteLine("Humidity (Station): " + humidity.ToString() + " %RH")
        Console.WriteLine("Wind Speed (Station): " + (windSpeed/10.0).ToString() + " m/s")
        Console.WriteLine("Gust Speed (Station): " + (gustSpeed/10.0).ToString() + " m/s")
        Console.WriteLine("Rain (Station): " + (rain/10.0).ToString() + " mm")
        Console.WriteLine("Wind Direction (Station): " + windDirection.ToString())
        Console.WriteLine("Battery Low (Station): " + batteryLow.ToString())
        Console.WriteLine("")
    End Sub

    ' Callback subroutine for sensor data callback
    Sub SensorDataCB(ByVal sender As BrickletOutdoorWeather, ByVal identifier As Byte, _
                     ByVal temperature As Short, ByVal humidity As Byte)
        Console.WriteLine("Identifier (Sensor): " + identifier.ToString())
        Console.WriteLine("Temperature (Sensor): " + (temperature/10.0).ToString() + " °C")
        Console.WriteLine("Humidity (Sensor): " + humidity.ToString() + " %RH")
        Console.WriteLine("")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim ow As New BrickletOutdoorWeather(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Enable station data callbacks
        ow.SetStationCallbackConfiguration(True)

        ' Enable sensor data callbacks
        ow.SetSensorCallbackConfiguration(True)

        ' Register station data callback to subroutine StationDataCB
        AddHandler ow.StationDataCallback, AddressOf StationDataCB

        ' Register sensor data callback to subroutine SensorDataCB
        AddHandler ow.SensorDataCallback, AddressOf SensorDataCB

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
