#include <ESP32Servo.h>

// create four servo objects 
Servo servo1;

#define servo1Pin 12

#define minUs 550
#define maxUs 2000


void setup() {
	Serial.begin(9600);
  servo1.setPeriodHertz(50); 
}

void loop() {
  int pos = 0;      // position in degrees
  servo1.attach(servo1Pin);
  for (pos = minUs; pos <= maxUs; pos += 1) { // sweep from 0 degrees to 180 degrees
    servo1.writeMicroseconds(pos);
		delay(5);
	}
	for (pos = maxUs; pos >= minUs; pos -= 1) { // sweep from 180 degrees to 0 degrees
		servo1.write(pos);
    servo1.writeMicroseconds(pos);
		delay(5);   
	}
  Serial.println("ree");
}