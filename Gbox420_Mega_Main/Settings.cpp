/**
EEPROM related functions - Persistent storage between reboots
Use cautiously, EEPROM has a write limit of 100.000 cycles 
Only use these in the setup() function, or when a user initiated change is stored 
 \attention Do not put these functions in the loop, EEPROM has a write limit of 100.000 cycles
*/
#include "Arduino.h"
#include "Settings.h"
#include "SerialLog.h"

void saveSettings(Settings *ToSave)
{                                                                                 
  eeprom_update_block((const void *)ToSave, (void *)0, sizeof(Settings)); ///update_block only writes the bytes that changed
}

Settings *loadSettings(bool ResetEEPROM = false )   ///if the function contains arguments with default values, they must be declared strictly before they are called, otherwise there is a compilation error: '<function name> was not declared in this scope. https://forum.arduino.cc/index.php?topic=606678.0
{
  Settings *DefaultSettings = new Settings();                                    ///This is where settings are stored, first it takes the sketch default settings defined in Settings.h
  Settings EEPROMSettings;                                                       ///temporary storage with "Settings" type
  eeprom_read_block((void *)&EEPROMSettings, (void *)0, sizeof(Settings)); ///Load EEPROM stored settings into EEPROMSettings
  if (DefaultSettings->CompatibilityVersion != EEPROMSettings.CompatibilityVersion || ResetEEPROM)
  { ///Making sure the EEPROM loaded settings are compatible with the sketch
    logToSerials(F("Incompatible stored settings detected, updating EEPROM..."), false, 1);
    saveSettings(DefaultSettings); ///overwrites EEPROM stored settings with defaults from this sketch
  }
  else
  {
    logToSerials(F("Same settings version detected, applying EEPROM settings..."), false, 1);
    ///DefaultSettings = EEPROMSettings; ///overwrite sketch defaults with loaded settings
    memcpy(DefaultSettings, &EEPROMSettings, sizeof(Settings));
  }
  logToSerials(F("done"), true, 1);
  return DefaultSettings;
}

void restoreDefaults(Settings *ToOverwrite)
{
  logToSerials(F("Forcing settings update at next restart..."), false, 0);
  ToOverwrite -> CompatibilityVersion = ToOverwrite -> CompatibilityVersion - 1;  
  saveSettings(ToOverwrite);
  logToSerials(F("done. Reseting Arduino..."), true, 1);
  resetFunc();  
}

void(* resetFunc) (void) = 0;  //Resets the Arduino https://forum.arduino.cc/index.php?topic=385427.0

