#include <NewPing.h>
#include <XBee.h>

#define echoPin 11 // Echo Pin
#define trigPin 12 // Trigger Pin
#define LEDPin 13 // Onboard LED

int maximumRange = 300; // Maximum range needed
int minimumRange = 0; // Minimum range needed
int motionThreshold = 200;
long duration, distance, lastReading, diff; // Duration used to calculate distance

// Create an XBee object at the top of your sketch
XBee xbee = XBee();

// Specify the address of the remote XBee (this is the SH + SL)
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40682fb6);

void setup() {
 Serial.begin (9600);
 xbee.setSerial(Serial);
 
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
 lastReading = 0;
}

void loop() {
/* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 
 diff = lastReading - distance;
 diff = abs(diff);
 lastReading = distance;
 
 if (distance <= maximumRange && distance >= minimumRange && diff > motionThreshold){
   // Create an array for holding the data you want to send.
  
  uint8_t payload[] = { 'M' };
  
  // Create a TX Request
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

  // Send your request
  xbee.send(zbTx);
 }
 
 //Delay 50ms before next reading.
 delay(50);
}

