#pragma once
//This is a template for creating new modules

//#include "3rdPartyModlue.h" //Load any required 3rd party modules here
#include "420Common_Web.h"
#include "../Modules/420Module_Web.h"
#include "../Components/ModuleSkeleton.h"

class ModuleSkeleton_Web : public ModuleSkeleton, public Common_Web
{
public:
  ModuleSkeleton_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ModuleSkeletonSettings *DefaultSettings); //constructor
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void websiteEvent_Field(char *Field);
  
private:
  
protected:
  Module_Web *Parent;
};