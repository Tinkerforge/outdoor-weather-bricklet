program ExampleCallback;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletOutdoorWeather;

type
  TExample = class
  private
    ipcon: TIPConnection;
    ow: TBrickletOutdoorWeather;
  public
    procedure StationDataCB(sender: TBrickletOutdoorWeather; const identifier: byte;
                            const temperature: smallint; const humidity: byte;
                            const windSpeed: longword; const gustSpeed: longword;
                            const rain: longword; const windDirection: byte;
                            const batteryLow: boolean);
    procedure SensorDataCB(sender: TBrickletOutdoorWeather; const identifier: byte;
                           const temperature: smallint; const humidity: byte);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change XYZ to the UID of your Outdoor Weather Bricklet }

var
  e: TExample;

{ Callback procedure for station data callback }
procedure TExample.StationDataCB(sender: TBrickletOutdoorWeather; const identifier: byte;
                                 const temperature: smallint; const humidity: byte;
                                 const windSpeed: longword; const gustSpeed: longword;
                                 const rain: longword; const windDirection: byte;
                                 const batteryLow: boolean);
begin
  WriteLn(Format('Identifier (Station): %d', [identifier]));
  WriteLn(Format('Temperature (Station): %f °C', [temperature/10.0]));
  WriteLn(Format('Humidity (Station): %d %%RH', [humidity]));
  WriteLn(Format('Wind Speed (Station): %f m/s', [windSpeed/10.0]));
  WriteLn(Format('Gust Speed (Station): %f m/s', [gustSpeed/10.0]));
  WriteLn(Format('Rain (Station): %f mm', [rain/10.0]));
  WriteLn(Format('Wind Direction (Station): %d', [windDirection]));
  WriteLn(Format('Battery Low (Station): %d', [batteryLow]));
  WriteLn('');
end;

{ Callback procedure for sensor data callback }
procedure TExample.SensorDataCB(sender: TBrickletOutdoorWeather; const identifier: byte;
                                const temperature: smallint; const humidity: byte);
begin
  WriteLn(Format('Identifier (Sensor): %d', [identifier]));
  WriteLn(Format('Temperature (Sensor): %f °C', [temperature/10.0]));
  WriteLn(Format('Humidity (Sensor): %d %%RH', [humidity]));
  WriteLn('');
end;

procedure TExample.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  ow := TBrickletOutdoorWeather.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Enable station data callbacks }
  ow.SetStationCallbackConfiguration(true);

  { Enable sensor data callbacks }
  ow.SetSensorCallbackConfiguration(true);

  { Register station data callback to procedure StationDataCB }
  ow.OnStationData := {$ifdef FPC}@{$endif}StationDataCB;

  { Register sensor data callback to procedure SensorDataCB }
  ow.OnSensorData := {$ifdef FPC}@{$endif}SensorDataCB;

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
