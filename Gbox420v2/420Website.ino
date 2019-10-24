#include "420Helpers.h"
#include "GrowBox.h"
#include "DHTSensor.h"
#include "Lights.h"
#include "Sound.h"
#include "PowerSensor.h"
#include "LightSensor.h"
#include "Aeroponics_NoTank.h"
#include "Aeroponics_Tank.h"

void resetWebServer(void) {    // Callback made from esp-link to notify that it has just come out of a reset
  logToSerials(F("WebServer (re)starting..."),false,0);
  while(!ESPLink.Sync())  {
    logToSerials(F("."),false,0);
    delay(500); 
    };
  //RestAPI.begin("api.pushingbox.com"); //Pre-setup relay to Google Sheets 
  WebServer.setup();
  URLHandler *GrowBoxHandler = WebServer.createURLHandler("/GrowBox.html.json"); //setup handling request from GrowBox.html
  URLHandler *SettingsHandler = WebServer.createURLHandler("/Settings.html.json"); //setup handling request from Settings.html
  GrowBoxHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  GrowBoxHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  GrowBoxHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  GrowBoxHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  SettingsHandler->loadCb.attach(&loadCallback);  //Called then the website loads initially
  SettingsHandler->refreshCb.attach(&refreshCallback); //Called periodically to refresh website content 
  SettingsHandler->buttonCb.attach(&buttonPressCallback); //Called when a button is pressed on the website
  SettingsHandler->setFieldCb.attach(&setFieldCallback); //Called when a field is changed on the website
  logToSerials(F("WebServer started"),true);
}

void loadCallback(char * url) //called when website is loaded.
{
  //if (strcmp(url,"/GrowBox.html.json")==0){
  // WebServer.setArgBoolean(F("AutoInternalFan"),GBox -> BoxSettings -> AutomaticInternalFan);
  // WebServer.setArgString(F("InternalFanSwitchTemp"), toText(GBox -> BoxSettings -> InternalFanSwitchTemp));
  // WebServer.setArgBoolean(F("AutoExhaustFan"), GBox -> BoxSettings -> AutomaticExhaustFan);
  // WebServer.setArgString(F("ExhaustFanHighHumid"), toText(GBox -> BoxSettings -> ExhaustFanHighHumid));
  // WebServer.setArgString(F("ExhaustFanLowHumid"), toText(GBox -> BoxSettings -> ExhaustFanLowHumid));
  // WebServer.setArgString(F("ExhaustFanOffHumid"), toText(GBox -> BoxSettings -> ExhaustFanOffHumid));
  for(int i=0;i<GBox -> WebsiteQueueLength_Load;i++){
    GBox -> WebsiteQueue_Load[i] -> websiteLoadEvent(url);
  } 
//}  
}

void refreshCallback(char * url) //called when website is refreshed.
{ 
  WebServer.setArgString(F("Time"), getFormattedTime());
  // WebServer.setArgString(F("td_LightOK"), statusToText(LightOK)); 
  // WebServer.setArgString(F("td_PressureOK"),statusToText(PressureOK));
  // WebServer.setArgString(F("td_PumpOK"),statusToText(PumpOK));
  // WebServer.setArgString(F("td_ACPowerOK"),statusToText(ACPowerOK));
  // WebServer.setArgString(F("td_DCPowerOK"),statusToText(DCPowerOK));
  // WebServer.setArgString(F("td_VentOK"),statusToText(VentOK));
  //WebServer.setArgString(F("td_ReservOK"),statusToText(ReservOK));
  for(int i=0;i<GBox -> WebsiteQueueLength_Refresh;i++){
    GBox -> WebsiteQueue_Refresh[i] -> websiteRefreshEvent(url);
  }
  //}
}

void buttonPressCallback(char *button)  //Called when any button on the website is pressed.
{
  if(GBox -> BoxSettings -> DebugEnabled)logToSerials(&button,true,0);
  for(int i=0;i<GBox -> WebsiteQueueLength_Button;i++){
    GBox -> WebsiteQueue_Button[i] -> websiteButtonPressEvent(button);
  }
  if (strcmp_P(ShortMessage,(PGM_P)F("RestoreDefaults"))==0) { restoreDefaults(GBox -> BoxSettings); }  
  saveSettings(false,GBox -> BoxSettings); 
}

void setFieldCallback(char * field){  //Called when any field on the website is updated.
  if(GBox -> BoxSettings -> DebugEnabled)logToSerials(&field,true,0);   
  for(int i=0;i<GBox -> WebsiteQueueLength_Field;i++){
    GBox -> WebsiteQueue_Field[i] -> websiteFieldSubmitEvent(field);
  }     
  saveSettings(false,GBox -> BoxSettings);
} 
