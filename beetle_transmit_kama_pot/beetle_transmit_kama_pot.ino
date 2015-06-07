


int potPin = 3;
   
   
void setup() {
 
    Serial.begin(115200);               //initial the Serial for Bluetooth and Serial USB Monitor
   
	//use potPin to read potentiometer input 
    pinMode (potPin, INPUT);
    delay (2000);
}
 
 
void loop()
{
	//convert from [0..1024] to [0..256]

	//connect a potentiometer to ground, 5V and analog pin 3
      int potValue = analogRead(potPin)>>2;

	//send the value over Bluetooth BLE      
       Serial.write(potValue);

	//don't send too often, otherwise overflooding buffers will cause delay/latency

       delay(50);
}
