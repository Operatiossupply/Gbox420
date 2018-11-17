void readSensors(){  //Sensor readings  
  Humidity = TempSensor.readHumidity();
  BoxTempC= TempSensor.readTemperature();
  BoxTempF = BoxTempC *1.8 + 32;
  isBright = !digitalRead(LightSensorInPin);     // read the input pin: 0- light detected , 1 - no light detected. Had to invert signal from the sensor to make more sense.
  LightReading = 1023 - analogRead(LightSensorAnalogInPin);
  LightReadingPercent = map(LightReading,MinLightReading,MaxLightReading,0,100);
  Voltage = PowerSensor.voltage(PowerSensorIP);
  Current = PowerSensor.current(PowerSensorIP);
  Power = PowerSensor.power(PowerSensorIP);
  Energy = PowerSensor.energy(PowerSensorIP) / 1000;  //total power consumption in kWh
  ReadPH();
  ReadMoisture();
  isWaterAboveCritical = !digitalRead(WaterCriticalInPin);
  isWaterAboveLow = !digitalRead(WaterLowInPin);
  isWaterAboveMedium = !digitalRead(WaterMediumInPin);
  isWaterAboveFull = !digitalRead(WaterFullInPin);
  readAeroPressure();
}

//***PH METER***
//Global variables for ReadPH funciton
int SensorValue = 0; 
unsigned long int Average;
int Results[10];

void ReadPH(){
  for(int i=0;i<10;i++) { //take 10 readings
   Results[i]=analogRead(PHMeterInPin);
  }
  Average=0;
  for(int i=0;i<10;i++) 
    Average+=Results[i];  //average of the 10 reads
  float PHVoltage=(float)Average*5.0/1024/10;
  PH = -5.70 * PHVoltage + 21.34;
}

//***PC SPEAKER***
void OnSound(){
  if(MySettings.isSoundEnabled){ 
  Serial.print("OnSound");
  tone(BuzzerOutPin,500);
  delay(100);
  noTone(BuzzerOutPin);
  tone(BuzzerOutPin,1000);
  delay(100);
  noTone(BuzzerOutPin);
  }
} 

void OffSound(){
  if(MySettings.isSoundEnabled){ 
  Serial.print("OffSound");
  tone(BuzzerOutPin,1000);
  delay(100);
  noTone(BuzzerOutPin);
  tone(BuzzerOutPin,500);
  delay(100);
  noTone(BuzzerOutPin);
  }
} 

//***Moisture sensor***
void ReadMoisture(){
  Moisture= analogRead(AMoistureSensorInPin);
  isWet = !digitalRead(DMoistureSensorInPin);
  if(Moisture > MaxMoisture) MaxMoisture = Moisture;
  if(Moisture < MinMoisture) MinMoisture = Moisture;
  MoisturePercentage = map(Moisture,MaxMoisture,MinMoisture,100,0);
}
