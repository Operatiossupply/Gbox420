#pragma once

//Aeroponics pins
  byte AeroSpraySolenoidPin = 22;  //Power relay Port 1 - Aeroponics spray solenoid (Only used with Aeroponics_Tank module)
  byte AeroBypassSolenoidPin = 23; //Power relay Port 3 - Aeroponics bypass solenoid 
  byte AeroPumpPin = 24;  //Power relay Port 2 - Aeroponics high pressure pump



//LightSensor pins
  byte LightSensor1DigitalPin = 36; //D0 - LM393 light sensor
  byte LightSensor1AnalogPin = A0; //A0 - LM393 light sensor   //A0 is just a variable that contains the actual pin number - https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h#L56-L72


//Sound pins
  byte Sound1Pin = 4; //PC speaker+ (red)

//PHSensor pins
  byte PHSensorInPin = A3;  //PH meter - Po port


//Analog pins
  byte PressureSensorInPin = A1; //Signal(yellow) - Pressure sensor
  byte WaterCriticalInPin = A4; //Water sensor1
  byte WaterLowInPin = A5;     //Water sensor2
  byte WaterMediumInPin = A6; //Water sensor3
  byte WaterFullInPin = A7; // Water sensor4
  
//Digital pins

  byte BuiltInLEDOutPin = 13;  //Built-in LED light for testing
  byte Relay4OutPin = 25;  //Power relay Port 4 - Internal fan Off/On
  byte Relay5OutPin = 26;  //Power relay Port 5 - Internal fan Low/High
  byte Relay6OutPin = 27;  //Power relay Port 6 - Exhaust fan Off/On
  byte Relay7OutPin = 28;  //Power relay Port 7 - Exhaust fan Low/High
  byte ATXPowerONOutPin = 34; //Turns ATX power supply on by connecting ATX PowerON pin to GND through an optocoupler
  byte ATXPowerGoodInPin = 35; //5V signal from ATX powersupply, inverted by optocoupler: LOW if DC power output is OK
  byte ReservoirTempSensorInPin = 45;  //Data(yellow) - DS18B20 waterproof temp sensor 
