#include <Arduino.h>
#include <ESP32Servo.h>

// create four servo objects 
Servo servo1;

#define servo1Pin 10

#define minUs 550
#define maxUs 2000

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 150

#define DIR 13
#define STEP 12

/*
 * Choose one of the sections below that match your board
 */

#include "A4988.h"
A4988 stepper(MOTOR_STEPS, DIR, STEP);

void setup() {
    /*
     * Set target motor RPM.
     */
    Serial.begin(9600);
    stepper.begin(RPM);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
    stepper.setSpeedProfile(stepper.LINEAR_SPEED, 6000, 6000);
    stepper.enable();
    servo1.setPeriodHertz(50); 
}

void loop() {
    delay(1000);

    /*
     * Moving motor in full step mode is simple:
     */
    stepper.setMicrostep(16);  // Set microstep mode to 1:1
    stepper.rotate(360 * 2);   // forward revolution
    stepper.rotate(-360 * 2);  // reverse revolution
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
}