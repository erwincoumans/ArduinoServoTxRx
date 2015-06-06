//receiver sketch, tested on Bluno Beetle (Arduino Uno)
//with Bluetooth BLE and servo on PWM Digital pin 3


#include <Servo.h>

//time out to go to neutral, in millisecond
#define LOST_CONNECTION_TIME_OUT_MS 500

Servo servo;

//servo value in range [0..180], 90 is neutral, 0 is full 'reverse', 
//180 full forward
int servoNeutral = 90;

void setup() {
  
  //start servo in neutral position
  servo.write(servoNeutral);

  //connect to digital PWM pin3 (for example Bluno Beetle D3 PWM)
  servo.attach(3);

///this Serial is both for Bluetooth BLE and Serial USB monitor
  Serial.begin(115200);               //initial the Serial
    
}
 
//time-out timers for disconnection and Serial print
int lastConnection = 0;
int printCount = 0;

void loop()
{
  lastConnection++;
  printCount++;
  
    if(Serial.available())
    {
      lastConnection = 0;
        int val = Serial.read();    //send what has been received
        
        int servoVal = map(val, 0, 255, 0, 179);
        servo.write(servoVal);
        if (printCount>100)
        {
          printCount = 0;
          Serial.print("Serial input:");
          Serial.println(val);
        }
     //   Serial.print("servoVal:");
     //   Serial.println(servoVal);
    } else
    {
      if (lastConnection>LOST_CONNECTION_TIME_OUT_MS)
      {
        //lost connection to transmitter, go to neutral
       servo.write(servoNeutral);
        
        //don't flood the Serial channel, print each 100ms maximum
        if (printCount>100)
        {
          Serial.println("LOST reception!");
        }
      }
    }
    
    ///wait 1ms for time-out timers
    delay(1);        
 
}
