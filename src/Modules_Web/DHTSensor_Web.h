#pragma once

#include "DHT.h" //DHT11 or DHT22 Digital Humidity and Temperature sensor
#include "../Modules/DHTSensor.h"

class GrowBox; //forward declaration

class DHTSensor_Web : public DHTSensor
{
public:
  DHTSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::DHTSensorSettings *DefaultSettings);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);  

private:
  GrowBox *GBox;
  
protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>DHT sensor</h1>		
  <table class="tg">				  
    <tr>
      <td class="headerRows"><span style="font-weight:bold">Internal<br>temp</span></td>
      <td class="headerRows"><span style="font-weight:bold">Internal<br>humidity</span></td>
      <td class="headerRows"><span style="font-weight:bold">External<br><span>temp</td>
      <td class="headerRows"><span style="font-weight:bold">External<br>humidity</span></td>
    </tr>
    <tr>
      <td id="InDHT_Temp" class="dataRows"></td>
      <td id="InDHT_Humidity" class="dataRows"></td>
      <td id="ExDHT_Temp" class="dataRows"></td>
      <td id="ExDHT_Humidity" class="dataRows"></td>
    </tr>
  </table>						
</div>		
*/