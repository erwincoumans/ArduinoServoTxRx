#include "VescController.h"

//Declare a controller object and tell it to use Serial1 port to communicate with VESC.
//Other serial ports on Arduino are called Serial, Serial2 and Serial3
//On some arduinos, like Arduino Nano, only Serial is available

VescController vesc(Serial);

void setup()
{
  Serial.begin(115200);
}

void loop()
{
   vesc.SetCurrent(3.00);
   delay(2000);

   vesc.SetRpm(1000);
   delay(2000);

   vesc.SetDuty(0.04);
   delay(2000);

   vesc.SetBrake(3.0);
   delay(2000);

   vesc.Release();
   delay(2000);

   void FullBrake();
   delay(2000);
}
