#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings,WeightSensor *BucketWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  StartWeight = &DefaultSettings->StartWeight;
  StopWeight = &DefaultSettings->StopWeight;
  SeedlingMode = &DefaultSettings->SeedlingMode;
  SeedlingWaterFreqency = &DefaultSettings->SeedlingWaterFreqency;
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("Hempy bucket object created"), true, 1);
}

void HempyBucket::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  checkBucketWeight();
  checkWateringFrequency();
}

void HempyBucket::refresh_Sec()
{
 if(BucketPump -> getOnState()) checkBucketWeight(); ///When pump is on check if it is time to stop
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Start weight:"));
  strcat(LongMessage, toText_weight(*StartWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Stop weight:"));
  strcat(LongMessage, toText_weight(*StopWeight));
  strcat_P(LongMessage, (PGM_P)F(" ; Seedling mode:"));
  strcat(LongMessage, toText_enabledDisabled(*SeedlingMode));
  logToSerials(&LongMessage, true, 1);
}

void HempyBucket::checkBucketWeight()
{  
  BucketWeightSensor -> readWeight();  //Force a weight refresh   
  if(BucketWeightSensor -> getWeight() > *StopWeight && BucketPump -> getOnState()) ///If the weight is over the limit and the pump is on
  {
    BucketPump -> stopPump();     
  }
  else if(BucketWeightSensor -> getWeight() < *StartWeight && !BucketPump -> getOnState() && BucketPump -> getEnabledState())  ///If the weight is below the limit AND the pump is off AND pump is enabled
  {
    BucketPump -> startPump(); 
    WaterTimer = millis();
  }   
}

void HempyBucket::checkWateringFrequency(){
    if(*SeedlingMode){
       if (millis() - WaterTimer >= ((uint32_t)*SeedlingWaterFreqency * 60000)){
         logToSerials(F("Watering seedling..."), true, 1);
         BucketPump -> startPump();
         WaterTimer = millis();
       }            
    }
}

float HempyBucket::getStartWeight()
{  
    return *StartWeight;
}

float HempyBucket::getStopWeight()
{
    return *StopWeight;
}


char *HempyBucket::getStartWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(*StartWeight);
  else
    return toText(*StartWeight);
}

char *HempyBucket::getStopWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_weight(*StopWeight);
  else
    return toText(*StopWeight);
}

void HempyBucket::setStartWeight(float Weight)
{
  if(*StartWeight != Weight)
  {
    *StartWeight = Weight;
    Parent -> getSoundObject() -> playOnSound();
  }
}

void HempyBucket::setStopWeight(float Weight)
{
  if(*StopWeight != Weight)
  {
    *StopWeight = Weight;
    logToSerials(Name, false, 1);
    logToSerials(F("Watering limits updated"), true, 1);
    Parent -> getSoundObject() -> playOnSound();
  }
}