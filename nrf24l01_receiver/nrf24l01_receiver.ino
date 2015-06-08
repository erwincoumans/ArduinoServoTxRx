
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h> 

int neutralServo = 77;
int rangeServo = 70;

//nRF24L01+ CE and CSN pins
RF24 radio(9,10);

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xe7e7e7e7e7LL;//0xE8E8F0F0E1LL;


static uint32_t message_count = 1;

//#define USE_SERIAL


Servo servo;

void setup()
{
#ifdef  USE_SERIAL
   Serial.begin(115200);   // First open the serial connection via the USB
 //  while (!Serial) {}
    Serial.println("Started!");
#endif


radio.begin();

  // We will be using the Ack Payload feature, so please enable it
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.writeAckPayload( 1, &message_count, sizeof(message_count) );
 
 servo.attach(3);
 servo.write(neutralServo);
 
 radio.openReadingPipe(1,pipe);
 
 radio.startListening();

#ifdef USE_SERIAL
  radio.printDetails();
#endif//USE_SERIAL

 
}

int threshold = 3;

const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

unsigned char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

unsigned long bla;
struct MyStatus
{
  int message_count;
  char potValueA;
  char potValueB;
  char unused1;
  char unused2;
  

};

int timeout = 0;
int targetValueA = 0;

void loop()
{
    timeout++;

    if (timeout>100)
    {
      servo.write(neutralServo);
 #ifdef USE_SERIAL
    Serial.println("Timeout!");
 #endif
    }
 

  {
   
    // if there is data ready
    if ( radio.available() )
    {
     // printf("radio available\n");
      // Dump the payloads until we've gotten everything
      static unsigned long got_time;
      bool done = false;

    bool dynamicPayload = true;
      if (dynamicPayload)
      {
         while (!done)
        {
          // Fetch the payload, and see if this was the last one.
  	int len = radio.getDynamicPayloadSize();
  	done = radio.read( receive_payload, len );
 // 	done = radio.read( &bla, len );
 
  	// Put a zero at the end for easy printing
  	receive_payload[len] = 0;
  
       //some simple 'checksum'
        if (receive_payload[0]==238)
       { 
         
  	targetValueA = map(receive_payload[1],0,255,neutralServo-rangeServo,neutralServo+rangeServo);
 #ifdef USE_SERIAL
      Serial.print("receive=");
      Serial.println(receive_payload[1]);
      Serial.print("servo=");
      Serial.println(targetValueA);
#endif

   servo.write(targetValueA);//neutralServo+(targetValueA-128.)/2.);
       
   timeout=0;
       }
  
  //Serial.println(targetValueA);
      // Spew it
  #ifdef USE_SERIAL
  	printf("Got dynamic payload size=%d:\n\r",len);
        for (int i=0;i<len;i++)
        {
          int v = receive_payload[i];
          printf("value[%d]=%d\n",i,v);
        }
  #endif //USE_SERIAL
        }
      } else
      {
        while (!done)
        {
          // Fetch the payload, and see if this was the last one.
          done = radio.read( &got_time, sizeof(unsigned long) );
#ifdef USE_SERIAL  
          // Spew it
          printf("Spew Got payload %lu\n",got_time);
#endif//USE_SERIAL
        }
      }

      // Add an ack packet for the next time around.  This is a simple
      // packet counter
      MyStatus stat;
      stat.message_count = message_count;
      stat.potValueA = targetValueA;
      stat.potValueB = targetValueA;
      stat.unused1 = 2;
      stat.unused2 = sizeof(int);
      radio.writeAckPayload( 1, &stat, sizeof(MyStatus) );
      ++message_count;
    }
  }
  
}

