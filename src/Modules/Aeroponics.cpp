#include "Aeroponics.h"
#include "../GrowBox.h"

// This is a virtual class (cannot be instanciated), parent of two classes:
// -Aeroponics_NoTank: High pressure pump is directly connected to the aeroponics tote
// -Aeroponics_Tank: A pressure tank is added between the high pressure pump and aeroponics tote, requires an extra solenoid for spraying

Aeroponics::Aeroponics(const __FlashStringHelper *Name, GrowBox *GBox, Settings::AeroponicsSettings *DefaultSettings, PressureSensor *FeedbackPressureSensor) : Common(Name)
{ //constructor
  this->GBox = GBox;
  BypassSolenoidPin = &DefaultSettings->BypassSolenoidPin;
  PumpPin = &DefaultSettings->PumpPin;
  SprayEnabled = &DefaultSettings->SprayEnabled; //Enable/disable misting
  Interval = &DefaultSettings->Interval;         //Aeroponics - Spray every 15 minutes
  Duration = &DefaultSettings->Duration;         //Aeroponics - Spray time in seconds
  PumpTimeout = &DefaultSettings->PumpTimeout;   // Aeroponics - Max pump run time in minutes, measue zero to max pressuretank refill time and adjust accordingly
  PrimingTime = &DefaultSettings->PrimingTime;   // Aeroponics - At pump startup the bypass valve will be open for X seconds to let the pump cycle water freely without any backpressure. Helps to remove air.
  this->FeedbackPressureSensor = FeedbackPressureSensor;
  pinMode(*BypassSolenoidPin, OUTPUT);
  digitalWrite(*BypassSolenoidPin, HIGH); //initialize in off state
  pinMode(*PumpPin, OUTPUT);
  digitalWrite(*PumpPin, HIGH);          //initialize in off state
  GBox->AddToReportQueue(this);          //Subscribing to the report queue: Calls the report() method
  GBox->AddToRefreshQueue_Sec(this);     //Subscribing to the 1 sec refresh queue: Calls the refresh_Sec() method
  GBox->AddToWebsiteQueue_Load(this);    //Subscribing to the Website load event: Calls the websiteEvent_Load() method
  GBox->AddToWebsiteQueue_Refresh(this); //Subscribing to the Website refresh event: Calls the websiteEvent_Refresh() method
  GBox->AddToWebsiteQueue_Button(this);  //Subscribing to the Website button press event: Calls the websiteEvent_Button() method
  GBox->AddToWebsiteQueue_Field(this);   //Subscribing to the Website field submit event: Calls the websiteEvent_Field() method
}

void Aeroponics::websiteEvent_Load(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgInt(getWebsiteComponentName(F("PumpTimeout")), *PumpTimeout);
    WebServer.setArgInt(getWebsiteComponentName(F("PrimingTime")), *PrimingTime);
    WebServer.setArgInt(getWebsiteComponentName(F("Interval")), *Interval);
    WebServer.setArgInt(getWebsiteComponentName(F("Duration")), *Duration);
  }
}

void Aeroponics::websiteEvent_Refresh(__attribute__((unused)) char *url)
{
  if (strcmp(url, "/GrowBox.html.json") == 0)
  {
    WebServer.setArgString(getWebsiteComponentName(F("Pressure")), FeedbackPressureSensor->getPressureText(true, false));
    WebServer.setArgString(getWebsiteComponentName(F("SprayEnabled")), sprayStateToText());
    WebServer.setArgString(getWebsiteComponentName(F("PumpState")), pumpStateToText());
    WebServer.setArgString(getWebsiteComponentName(F("BypassState")), onOffToText(BypassSolenoidOn));
  }
}

void Aeroponics::websiteEvent_Button(char *Button)
{
  if (!isThisMyComponent(Button))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      setPumpOn(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0)
    {
      setPumpOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDisable")) == 0)
    {
      PumpDisable();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      Mix();
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEnable")) == 0)
    {
      setSprayOnOff(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDisable")) == 0)
    {
      setSprayOnOff(false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      sprayNow(true);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      sprayOff();
    }
  }
}

void Aeroponics::websiteEvent_Field(__attribute__((unused)) char *Field)
{ //When a field is submitted using the Set button
  if (strcmp_P(ShortMessage, (PGM_P)F("PumpTimeout")) == 0)
  {
    setPumpTimeout(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("PrimingTime")) == 0)
  {
    setPrimingTime(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("Duration")) == 0)
  {
    setDuration(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("Interval")) == 0)
  {
    setInterval(WebServer.getArgInt());
  }
  else if (strcmp_P(ShortMessage, (PGM_P)F("SprayEnabled")) == 0)
  {
    setSprayOnOff(WebServer.getArgBoolean());
  }
}

void Aeroponics::report()
{                                                  //report status to Serial output, runs after the child class`s report function
  memset(&LongMessage[0], 0, sizeof(LongMessage)); //clear variable
  strcat_P(LongMessage, (PGM_P)F(" ; SprayEnabled:"));
  strcat(LongMessage, yesNoToText(SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F(" ; Interval:"));
  strcat(LongMessage, toText(*Interval));
  strcat_P(LongMessage, (PGM_P)F(" ; Duration:"));
  strcat(LongMessage, toText(*Duration));
  strcat_P(LongMessage, (PGM_P)F(" ; PumpTimeout:"));
  strcat(LongMessage, toText(*PumpTimeout));
  strcat_P(LongMessage, (PGM_P)F(" ; PrimingTime:"));
  strcat(LongMessage, toText(*PrimingTime));
  logToSerials(&LongMessage, true, 0); //Break line, No indentation needed: child class already printed it
}

void Aeroponics::checkRelays()
{
  if (PumpOn)
    digitalWrite(*PumpPin, LOW);
  else
    digitalWrite(*PumpPin, HIGH);
  if (BypassSolenoidOn)
    digitalWrite(*BypassSolenoidPin, LOW);
  else
    digitalWrite(*BypassSolenoidPin, HIGH);
}

char *Aeroponics::pumpStateToText()
{
  if (!PumpOK)
    return (char *)"DISABLED";
  else
  {
    if (PumpOn)
      return (char *)"ON";
    else
      return (char *)"OFF";
  }
}

void Aeroponics::setPumpOn(bool UserRequest)
{ //turns pump on, UserRequest is true if it was triggered from the website
  if (UserRequest)
  { //if the pump was turned on from the web interface, not by the automation
    GBox->addToLog(F("Pump ON"));
    GBox->Sound1->playOnSound();
    PumpOK = true; //re-enable pump
  }
  MixInProgress = UserRequest; //If pump was turned on from the web interface first run an air bleeding cycle
  PumpOn = true;
  PumpTimer = millis();
  checkRelays();
}

void Aeroponics::setPumpOff(bool UserRequest)
{
  if (UserRequest)
  { //if the pump was turned off from the web interface, not by the automation
    GBox->addToLog(F("Pump OFF"));
    GBox->Sound1->playOffSound();
    PumpOK = true; //re-enable pump
  }
  MixInProgress = false;
  PumpOn = false;
  //if(!BlowOffInProgress)BypassSolenoidOn = false; //Close bypass valve
  PumpTimer = millis();
  checkRelays();
}

void Aeroponics::PumpDisable()
{
  setPumpOff(false);
  PumpOK = false;
  GBox->addToLog(F("Pump disabled"));
}

void Aeroponics::Mix()
{
  PumpOn = true;
  MixInProgress = true;
  PumpOK = true;
  BypassSolenoidOn = true;
  PumpTimer = millis();
  checkRelays();
  GBox->Sound1->playOnSound();
  GBox->addToLog(F("Mixing nutrients"));
}

void Aeroponics::setInterval(int interval)
{
  *Interval = interval;
  SprayTimer = millis();
}

char *Aeroponics::getInterval()
{
  return toText(*Interval);
}

void Aeroponics::setDuration(int duration)
{
  *Duration = duration;
  SprayTimer = millis();
  GBox->addToLog(F("Spray time updated"));
}

char *Aeroponics::getDuration()
{
  return toText(*Duration);
}

void Aeroponics::setPumpTimeout(int Timeout)
{
  *PumpTimeout = Timeout;
  GBox->addToLog(F("Aero pump timeout updated"));
}

void Aeroponics::setPrimingTime(int Timing)
{
  *PrimingTime = Timing;
  GBox->addToLog(F("Aero priming time updated"));
}

void Aeroponics::setSprayOnOff(bool State)
{
  *SprayEnabled = State;
  if (*SprayEnabled)
  {
    GBox->addToLog(F("Aero spray enabled"));
    GBox->Sound1->playOnSound();
  }
  else
  {
    GBox->addToLog(F("Aero spray disabled"));
    GBox->Sound1->playOffSound();
  }
}