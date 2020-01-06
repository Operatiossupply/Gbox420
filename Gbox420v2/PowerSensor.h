#pragma once

#include "PZEM004T.h" //3rd party module for the PZEM004T power sensor
#include "420Common.h"

class GrowBox;  //forward declaration

class PowerSensor : public Common
{  
  public:
    PowerSensor(const __FlashStringHelper * Name, GrowBox * Gbox,HardwareSerial * SerialPort);  //constructor  
    float Power; //Power sensor - W
    float Energy; //Power sensor - Wh Total consumption 
    float Voltage; //Power sensor - V
    float Current; //Power sensor - A
    
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void refresh_Minute();  //Called when component should refresh its state
    void report();
    char * getPowerText(bool IncludeUnits);
    char * getEnergyText(bool IncludeUnits);   
    char * getVoltageText(bool IncludeUnits);
    char * getCurrentText(bool IncludeUnits);    

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    PZEM004T * Sensor; //for PZEM004T model
    IPAddress * PowerSensorIP; // Power Sensor address (fake,just needs something set)  

  protected:
};