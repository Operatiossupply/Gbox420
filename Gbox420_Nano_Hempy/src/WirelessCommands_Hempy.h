#pragma once

#include "TimeLib.h"     ///< keeping track of time
///Structs for wireless communication

///Global constants
enum HempyMessages { Module1Command,Module1Response,Bucket1Command,Bucket1Response,Bucket2Command,Bucket2Response,GetNext};  ///< An enum has an underlying integer type (the type used to store the value of the enum), and the enum value can be implicitly converted to that integer type's value. https://stackoverflow.com/questions/10644754/is-passing-an-enum-value-to-an-int-parameter-non-standard/10644824
//enum MessageType { Command, Response };

static const __FlashStringHelper* toText_hempySequenceID(uint8_t SequenceID) 
{
   switch (SequenceID) 
   {
      case HempyMessages::Module1Command: return F("Module1Command"); break;
      case HempyMessages::Module1Response: return F("Module1Response"); break;
      case HempyMessages::Bucket1Command: return F("Bucket1Command"); break;
      case HempyMessages::Bucket1Response: return F("Bucket1Response"); break;
      case HempyMessages::Bucket2Command: return F("Bucket2Command"); break;
      case HempyMessages::Bucket2Response: return F("Bucket2Response"); break;
      case HempyMessages::GetNext: return F("GetNext"); break;
      default : return F("UNKNOWN"); break;
   }
}
                                                                                                                                         ///< GetNext should always be the last element
struct CommonTemplate  ///< Shared between Command and Respone packages
{
   HempyMessages SequenceID;  ///< Commands and Responses can span across multiple 32byte packages. Packages with 0 SequenceID represent the initial attempt to exchange data
   
   CommonTemplate(HempyMessages SequenceID){
      this -> SequenceID = SequenceID;
   }
};

///< Both the Transmitter and the Receiver needs to know these structures

///< Commands are sent by the Main module
struct ModuleCommand : CommonTemplate  //Max 32bytes. Module command sent by the Main module
{
    ModuleCommand(__attribute__((unused)) HempyMessages SequenceID) : CommonTemplate(SequenceID){}
    ModuleCommand(__attribute__((unused)) HempyMessages SequenceID,__attribute__((unused)) time_t Time,__attribute__((unused)) bool Debug,__attribute__((unused)) bool Metric) : CommonTemplate(SequenceID){}
    time_t Time = 0; 
    bool Debug = true;
    bool Metric = true;
};

struct ModuleResponse  : CommonTemplate  //Max 32bytes. Module response sent back to the Main module
{
   ModuleResponse(__attribute__((unused)) HempyMessages SequenceID) : CommonTemplate(SequenceID){}
   ModuleResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool Status) : CommonTemplate(SequenceID){}
   bool Status = true;   
};

struct BucketCommand : CommonTemplate  //Max 32bytes. Command to control one hempy bucket
{
    BucketCommand(__attribute__((unused)) HempyMessages SequenceID) : CommonTemplate(SequenceID){}
    BucketCommand(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool DisablePump, __attribute__((unused)) bool TurnOnPump, __attribute__((unused)) bool TurnOffPump, __attribute__((unused)) int TimeOutPump, __attribute__((unused)) float StartWeight, __attribute__((unused)) float StopWeight) : CommonTemplate(SequenceID){}   
    bool DisablePump = false;
    bool TurnOnPump = false;
    bool TurnOffPump = false;
    int TimeOutPump = 0;
    float StartWeight = 0.0;
    float StopWeight = 0.0;
};

struct BucketResponse  : CommonTemplate  //Max 32bytes. Response from one hempy bucket
{
   BucketResponse(__attribute__((unused)) HempyMessages SequenceID) : CommonTemplate(SequenceID){}
   BucketResponse(__attribute__((unused)) HempyMessages SequenceID, __attribute__((unused)) bool PumpOn,__attribute__((unused))  bool PumpEnabled,__attribute__((unused))  float Weight) : CommonTemplate(SequenceID){}
   PumpStates PumpState = DISABLED;
   float Weight = 0.0;
};