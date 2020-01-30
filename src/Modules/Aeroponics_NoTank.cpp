#include "Aeroponics_NoTank.h"
#include "../../GrowBox.h"

Aeroponics_NoTank::Aeroponics_NoTank(const __FlashStringHelper *Name, GrowBox *GBox, Settings::AeroponicsSettings *DefaultSettings, Settings::AeroponicsSettings_NoTankSpecific *NoTankSpecificSettings, PressureSensor *FeedbackPressureSensor) : Aeroponics(&(*Name), &(*GBox), &(*DefaultSettings), &(*FeedbackPressureSensor))
{
  BlowOffTime = &NoTankSpecificSettings->BlowOffTime; //Aeroponics - Turn on pump below this pressure (bar)
  logToSerials(F("Aeroponics_NoTank object created"), true, 1);
}

void Aeroponics_NoTank::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgInt(getWebsiteComponentName(F("BlowOffTime")), *BlowOffTime);
  }
  Aeroponics::websiteEvent_Load(url);
}

void Aeroponics_NoTank::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("SprayPressure")), pressureToText(LastSprayPressure));
  }
  Aeroponics::websiteEvent_Refresh(url);
}

void Aeroponics_NoTank::websiteEvent_Field(char *Field)
{
  if (!isThisMyComponent(Field))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("BlowOffTime")) == 0)
    {
      setBlowOffTime(WebServer.getArgInt());
    }
    else
      Aeroponics::websiteEvent_Field(Field);
  }
}

void Aeroponics_NoTank::refresh_Sec()
{ //pump directly connected to aeroponics tote, with an electronically controlled bypass valve
  if (GBox->BoxSettings->DebugEnabled)
    Common::refresh_Sec();

  if (BlowOffInProgress && millis() - SprayTimer >= ((uint32_t)*BlowOffTime * 1000))
  {                           //checking pressure blow-off timeout
    BypassSolenoidOn = false; //Close bypass valve
    BlowOffInProgress = false;
    logToSerials(F("Stopping blow-off"), true);
    Aeroponics::FeedbackPressureSensor->readPressure(); //Update pressure after closing bypass valve
  }
  if (PumpOn)
  { //if pump is on
    FeedbackPressureSensor->readPressure();
    if (millis() - PumpTimer >= ((uint32_t)*PumpTimeout * 60000))
    { //checking pump timeout
      setPumpOff(false);
      logToSerials(F("Pump timeout reached"), true);
    }

    if (!MixInProgress && !BypassSolenoidOn && PumpOn && millis() - SprayTimer >= ((uint32_t)*Duration * 1000))
    { //bypass valve is closed and time to stop spraying (Duration in Seconds)
      BypassSolenoidOn = true;
      BlowOffInProgress = true; //no extra timer is needed, will use SprayTimer
      checkRelays();
      setPumpOff(false);
      SprayTimer = millis();
      LastSprayPressure = Aeroponics::FeedbackPressureSensor->getPressure();
      logToSerials(F("Stopping spray"), true);
    }
    else
    {
      if (!MixInProgress && BypassSolenoidOn && millis() - PumpTimer >= ((uint32_t)*PrimingTime * 1000))
      {                           //self priming timeout reached, time to start spraying
        BypassSolenoidOn = false; //Close bypass valve
        checkRelays();
        SprayTimer = millis();
        Aeroponics::FeedbackPressureSensor->Pressure->resetAverage();
        logToSerials(F("Closing bypass, starting spray"), true);
      }
    }
  }
  else
  { //pump is off
    if (!BlowOffInProgress)
      BypassSolenoidOn = false; //Should not leave the solenoid turned on
    if (PumpOK && *SprayEnabled && ((millis() - SprayTimer) >= ((uint32_t)*Interval * 60000)))
    { //if time to start spraying (AeroInterval in Minutes)
      sprayNow(false);
    }
  }
  checkRelays();
}

void Aeroponics_NoTank::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F("LastSprayPressure:"));
  strcat(LongMessage, pressureToText(LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F(" ; BlowOffTime:"));
  strcat(LongMessage, toText(*BlowOffTime));
  logToSerials(&LongMessage, false, 1); //first print Aeroponics_Tank specific report, without a line break
  Aeroponics::report();                 //then print parent class report
}

void Aeroponics_NoTank::sprayNow(bool FromWebsite)
{
  if (*SprayEnabled || FromWebsite)
  {
    MixInProgress = false;
    PumpOK = true;
    PumpOn = true;
    BypassSolenoidOn = true;
    checkRelays();
    PumpTimer = millis();
    SprayTimer = millis();
    GBox->Sound1->playOnSound();
    if (FromWebsite)
      GBox->addToLog(F("Aeroponics spraying"));
    else
      logToSerials(F("Aeroponics spraying"), true, 3);
  }
}

void Aeroponics_NoTank::sprayOff()
{
  PumpOn = false;
  BypassSolenoidOn = false;
  checkRelays();
  GBox->addToLog(F("Aeroponics spray OFF"));
}

char *Aeroponics_NoTank::sprayStateToText()
{
  if (!*SprayEnabled)
    return (char *)"DISABLED";
  else
  {
    if (PumpOn && !BypassSolenoidOn)
      return (char *)"ON";
    else
      return (char *)"ENABLED";
  }
}

void Aeroponics_NoTank::setBlowOffTime(int _BlowOffTime)
{
  *BlowOffTime = _BlowOffTime;
  GBox->addToLog(F("Blowoff time updated"));
}