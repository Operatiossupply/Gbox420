#include "HempyBucket.h"

HempyBucket::HempyBucket(const __FlashStringHelper *Name, Module *Parent, Settings::HempyBucketSettings *DefaultSettings,WeightSensor *BucketWeightSensor, WaterPump *BucketPump) : Common(Name)
{
  this->Parent = Parent;
  this->BucketWeightSensor = BucketWeightSensor;
  this->BucketPump = BucketPump;
  StartWeight = &DefaultSettings->StartWeight;
  StopWeight = &DefaultSettings->StopWeight;
  Parent->addToReportQueue(this);         ///Subscribing to the report queue: Calls the report() method
  Parent->addToRefreshQueue_Sec(this);  
  Parent->addToRefreshQueue_Minute(this); ///Subscribing to the 1 minute refresh queue: Calls the refresh_Minute() method
  logToSerials(F("Hempy bucket object created"), true, 1);
}

void HempyBucket::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
  checkBucketWeight();
}

void HempyBucket::refresh_Sec()
{
 if(BucketPump -> getPumpOnStatus()) checkBucketWeight(); ///When watering check if its time to stop
}

void HempyBucket::checkBucketWeight()
{  
  BucketWeightSensor -> readWeight();  //Force a weight refresh   
  if(BucketWeightSensor -> getWeight() > *StopWeight && BucketPump -> getPumpOnStatus()) ///If the weight is over the limit and the pump is on
  {
    BucketPump -> turnOff();     
  }
  else if(BucketWeightSensor -> getWeight() < *StartWeight && !BucketPump -> getPumpOnStatus())  ///If the weight is below the limit and the pump is off
  {
    BucketPump -> turnOn(); 
  }   
}

void HempyBucket::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("Weight:"));
  strcat(LongMessage, BucketWeightSensor -> getWeightText(true));
  strcat_P(LongMessage, (PGM_P)F(" ["));
  strcat(LongMessage, toText(*StartWeight));
  strcat_P(LongMessage, (PGM_P)F("/"));
  strcat(LongMessage, toText(*StopWeight));
  strcat_P(LongMessage, (PGM_P)F("]"));
  logToSerials(&LongMessage, true, 1);
}

char *HempyBucket::getStartWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return weightToText(*StartWeight);
  else
    return toText(*StartWeight);
}

char *HempyBucket::getStopWeightText(bool IncludeUnits)
{
  if (IncludeUnits)
    return weightToText(*StopWeight);
  else
    return toText(*StopWeight);
}

void HempyBucket::setStartWeight(float Weight)
{
  *StartWeight = Weight;
}

void HempyBucket::setStopWeight(float Weight)
{
  *StopWeight = Weight;
}