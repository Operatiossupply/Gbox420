#pragma once
#include "420Module_Web.h"
#include "ELClientRest.h" //ESP-link - REST API

//This class represents a complete growbox with all of its components
//Responsible for setting up each module, updating their statuses and reporting it

//forward declaration of classes
class DHTSensor_Web;
class LightSensor_Web;
class Lights_Web;
class Sound_Web;
class Fan_Web;
class PowerSensor_Web;
class PowerSensorV3_Web;  //Only for PZEM004T V3.0
class PHSensor_Web;
class LightSensor_Web;
class PressureSensor_Web;
class Aeroponics_NoTank_Web;
class Aeroponics_Tank_Web;
class WaterTempSensor_Web;
class WaterLevelSensor_Web;
class WeightSensor_Web;
class ModuleSkeleton_Web; //Only for demonstration purposes

extern ELClientRest PushingBoxRestAPI;

class GrowBox : public Module_Web, public Common_Web
{
public:
  GrowBox(const __FlashStringHelper *Name, Settings::GrowBoxSettings *DefaultSettings); //constructor
  Sound_Web *Sound1;             //Pointer to a Piezo speaker - sound feedback
  Fan_Web *InFan;                //Internal fan
  Fan_Web *ExFan;                //Exhaust fan
  DHTSensor_Web *InDHT;          //Pointer to a Digital Humidity Sensor object measuring the internal temperature of the grow box
  DHTSensor_Web *ExDHT;          //Pointer to a Digital Humidity Sensor object measuring the external temperature of the grow box
  LightSensor_Web *LightSensor1; //Pointer to a Light Sensor object measuring light intensity in the grow box
  Lights_Web *Light1;            //Pointer to a Light assembly
  PowerSensor_Web *Power1;
  //PowerSensorV3_Web * Power1;  //Only for PZEM004T V3.0
  PressureSensor_Web *Pressure1;
  PHSensor_Web *PHSensor1;
  Aeroponics_Tank_Web *Aero_T1;
  Aeroponics_NoTank_Web *Aero_NT1;
  WaterTempSensor_Web *WaterTemp1;
  WaterLevelSensor_Web *WaterLevel1;
  WeightSensor_Web *Weight1; 
  WeightSensor_Web *Weight2;  
  ModuleSkeleton_Web *ModuleSkeleton1; //Only for demonstration purposes
  ModuleSkeleton_Web *ModuleSkeleton2; //Only for demonstration purposes
  void websiteEvent_Load(char *url);
  void websiteEvent_Refresh( char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  void refresh_FiveSec();
  void refresh_Minute();
  void refresh_QuarterHour();  
  void addToLog(const __FlashStringHelper *Text, byte indent = 3);
  void addToLog(const char *Text, byte indent = 3);
  void reportToGoogleSheetsTrigger();
  void reportToGoogleSheets(bool CalledFromWebsite);
  void relayToGoogleSheets(const __FlashStringHelper *Title, char (*JSONData)[MaxLongTextLength]);

private:
  void setSheetsReportingOnOff(bool State);
  void setSheetsReportingFrequency(int Frequency);
  void setDebugOnOff(bool State);
  void setMetricSystemEnabled(bool MetricEnabled);
  void setPushingBoxLogRelayID(const char *ID);
  char *eventLogToJSON(bool Append = false); //Creates a JSON array: ["Log1","Log2","Log3",...,"LogN"]

protected:
  bool RefreshAllRequested = false;
  bool ConsoleReportRequested = false;
  bool ReportToGoogleSheetsRequested = false;
  int *SheetsReportingFrequency;
  bool *ReportToGoogleSheets;
  int SheetsRefreshCounter = 0;
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>Settings</h1>
  <button id="GBox1_RestoreDefaults" type="button" style="margin-top:0px" title="Restore settings to sketch defaults">Restore defaults</button>
  <form><label><input type="checkbox" name="GBox1_DebugEnabled" title="Debug entres on Serial log">Debug enabled </label><input type="submit" value="Set"></form>
  <form><label><input type="checkbox" name="GBox1_MetricSystemEnabled" title="Use imperial units(F,PSI) or metric (C,bar)">Metric units </label><input type="submit" value="Set"></form>				
</div>

<div class="card" style="width:90%">
  <h1>Reporting</h1>					
  <form><label><input type="checkbox" name="GBox1_SheetsEnabled">Google Sheets, every <input style="width: 55px;" min=15 max=1440 step=15 name="GBox1_SheetsFrequency" type="number"/> min </label><input type="submit" value="Set"></form>
  <form>RelayID:<input type="text" name="GBox1_PushingBoxLogRelayID"  style="width: 150px;" title="DeviceID of the PushingBox logging scenario"/><input type="submit" value="Set"></form>
</div>

<div class="pure-u-1 pure-u-md-1-2" style="max-width:370px">
  <div class="card" style="width:90%">	
    <h1><span id="Time"></span></h1>				
    <ul id="list_SerialLog"></ul>
    <button id="GBox1_Refresh" type="button" style="margin-top:0px" title="Updates the status of every module">Refresh</button>	
  </div>					
</div>
*/