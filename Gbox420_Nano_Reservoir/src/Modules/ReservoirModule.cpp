/**@file*/
///Supports monitoring two Reservoir buckets
///Runs autonomously on an Arduino Nano RF and communicates wirelessly with the main module

#include "ReservoirModule.h"
#include "../Components/DHTSensor.h"
#include "../Components/Sound.h"
#include "../Components/PHSensor.h"
#include "../Components/WaterLevelSensor.h"
#include "../Components/WaterTempSensor.h"
#include "../Components/DistanceSensor.h"

ReservoirModule::ReservoirModule(const __FlashStringHelper *Name, Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Module()
{ 
  Sound1 = new Sound(F("Sound1"), this, &ModuleSettings->Sound1); ///Passing ModuleSettings members as references: Changes get written back to ModuleSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&ModuleSettings) + offsetof(Settings, VARIABLENAME))
  this -> SoundFeedback = Sound1;
  DHT1 = new DHTSensor(F("DHT1"), this, &ModuleSettings->DHT1);
  WTmp1 = new WaterTempSensor(F("WTmp1"), this, &ModuleSettings->WTmp1);
  WLev1 = new WaterLevelSensor(F("WLev1"), this, &ModuleSettings->WLev1);
  PHSen1 = new PHSensor(F("PHSen1"), this, &ModuleSettings->PHSen1); 
  Dist1 = new DistanceSensor(F("Dist1"), this, &ModuleSettings->Dist1);
  //addToRefreshQueue_Sec(this);         
  addToRefreshQueue_FiveSec(this);     
  //addToRefreshQueue_Minute(this);    
  //addToRefreshQueue_QuarterHour(this); 
  logToSerials(Name, false, 0);
  logToSerials(F("- ReservoirModule object created, refreshing..."), true, 1);
  runAll();
  addToLog(F("ReservoirModule initialized"), 0);
}

void ReservoirModule::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec(); 
  runReport(); 
  updateResponse(); 
}

void ReservoirModule::updateResponse(){
/*   Response.PumpState_B1 = Pump1 -> getState();;
  Response.Weight_B1 = Weight1 -> getWeight();
  Response.PumpState_B2 = Pump2 -> getState();
  Response.Weight_B2 = Weight2 -> getWeight();  */
  Wireless.flush_tx();  ///< Dump all previously cached but unsent ACK messages from the TX FIFO buffer (Max 3 are saved) 
  Wireless.writeAckPayload(1, &Response, sizeof(Response)); ///< load the payload to send the next time
}

void ReservoirModule::processCommand(reservoirCommand *Command){
  setDebug(Command -> Debug);
  setMetric(Command -> Metric);
/*   if(Command -> DisablePump_B1) Pump1 -> disablePump();
  if(Command -> TurnOnPump_B1) Pump1 -> startPump(true);
  if(Command -> TurnOffPump_B1) Pump1 -> stopPump();
  Pump1 -> setPumpTimeOut(Command -> TimeOutPump_B1);
  Bucket1 -> setStartWeight(Command -> StartWeightBucket_B1);
  Bucket1 -> setStopWeight(Command -> StopWeightBucket_B1);

  if(Command -> DisablePump_B2) Pump2 -> disablePump();
  if(Command -> TurnOnPump_B2) Pump2 -> startPump(true);
  if(Command -> TurnOffPump_B2) Pump2 -> stopPump();
  Pump2 -> setPumpTimeOut(Command -> TimeOutPump_B2);
  Bucket2 -> setStartWeight(Command -> StartWeightBucket_B2);
  Bucket2 -> setStopWeight(Command -> StopWeightBucket_B2); */

  updateResponse();
  saveSettings(ModuleSettings);

  if(*Debug){
    logToSerials(Command -> Debug,false,3);
      logToSerials(F(","),false,1);
      logToSerials(Command -> Metric,false,1);
      logToSerials(F(";"),false,1);
 /*    logToSerials(Command -> DisablePump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOnPump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOffPump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TimeOutPump_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StartWeightBucket_B1,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StopWeightBucket_B1,false,1);
        logToSerials(F(";"),false,1);
  logToSerials(Command -> DisablePump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOnPump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TurnOffPump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> TimeOutPump_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StartWeightBucket_B2,false,1);
        logToSerials(F(","),false,1);
        logToSerials(Command -> StopWeightBucket_B2,true,1); */
  }       
}