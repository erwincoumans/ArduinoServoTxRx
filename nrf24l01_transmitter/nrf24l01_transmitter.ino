#include <SPI.h>
#include "RF24.h"
#include "printf.h"


//nRF24L01+ radio, pin 9 = CE, pin 10 = CSN
RF24 radio(9,10);

// Single nRF24L01 pipe addresses for both transmitter and receiver
const uint64_t pipe = 0xe7e7e7e7e7LL;

//potValueA is the throttle potentiometer, potValueB is unused
int potValueA, potValueB;
int potPinA = 3;        
int potPinB = 2;

const int min_payload_size = 1;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

void setup(void)
{

  Serial.begin(115200);
  printf_begin();

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // enable dynamic payloads
  radio.setCRCLength( RF24_CRC_16 ) ;
  radio.enableDynamicPayloads();

   radio.enableAckPayload();
  
  // optionally, increase the delay between retries & # of retries
 // radio.setAutoAck( true ) ;
  radio.setPALevel( RF24_PA_HIGH ) ;

  pinMode (potPinA, INPUT);
  pinMode (potPinB, INPUT);
  
   radio.openWritingPipe(pipe);

  radio.startListening();

  radio.printDetails();
}

float time = 0;
int fail = 0;
int success=0;
int counter=0;
int ack=0;
int noack=0;

void loop(void)
{

  //read the value of the potentiometer, to be send to the receiver  
   potValueA = analogRead(potPinA)>>2;

   potValueB = analogRead(potPinB)>>2;
  // Serial.print("potValueA=");
   // Serial.println(potValueA);
  
    {
      static unsigned char send_payload[] = {238,0,0,0};
      next_payload_size = 4*sizeof(char);
      send_payload[1] = potValueA;
      send_payload[2] = 127.f+127.f*sin(time/122.);
  
      time+=2;
      
     if (radio.isAckPayloadAvailable() )
     {
        char data[4];
        radio.read(data,4*sizeof(char));
        ack++;
        if (counter>90)
        {
          Serial.print("\n\rGot Ack:");
         Serial.print((int)data[0]);
          Serial.print(" ,");
         Serial.print((int)data[1]);
          Serial.print(" ,");
  
         Serial.print((int)data[2]);
          Serial.print(" ,");
  
         Serial.println((int)data[3]);
          Serial.print(" ,");
  
        }
   } else
  {
    noack++;
  }

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    //printf("Now sending length %i...\n",next_payload_size);
    bool sendOK = radio.write( send_payload, next_payload_size, false );
    
    // Now, continue listening
    radio.startListening();
     
     if (sendOK)
     {     
       success++;
     } else
     {
       fail++;
     }
    
    counter++;
    if (counter>100)
    {
      counter=0;
      printf("success=%d, fail = %d, ack=%d, noack=%d\n", success, fail,ack,noack);
    }
  }


}

