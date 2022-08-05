#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// create a servo object
Servo servoTilt;
Servo servoPan;

#define servoTiltPin 15
#define servoPanPin 16

#define minUs 550
#define maxUs 2000

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define MICROSTEPS 16
#define RPM 150
// #define RPM 75

#define DIR 13
#define STEP 12

#define LEFT_STOP 2
#define RIGHT_STOP 14

#define BUILTIN_FLASH 4

#define TILT_LIMIT 135
#define TILT_90_DEG 62
// These two values refer to camera tilt relative to flat plane
#define TILT_MIN 28
#define TILT_MAX TILT_MIN + TILT_LIMIT

/*
 * Choose one of the sections below that match your board
 */

#include "A4988.h"
A4988 stepper(MOTOR_STEPS, DIR, STEP);

int count;  // Initialize to the movement we do back froscehamticm the wall

const char* ssid = "ESP_Cam";
const char* password = "29292929";

WebServer server(80);

boolean LEDStatus = LOW;

void handleLedOn() {
  LEDStatus = HIGH;
  digitalWrite(4, LEDStatus);
  server.send(200, "text/plain", "Led On"); 
}

void handleLedOff() {
  LEDStatus = LOW;
  digitalWrite(4, LEDStatus);
  server.send(200, "text/html", "Led On"); 
}

void handleNotFound() {
  server.send(404, "text/html", "Not Found"); 
}

void setupWebServer() {
	boolean result = WiFi.softAP(ssid, password, 13);
	if (result) {
		Serial.println("Ready");
	} else {
		Serial.println("Failed!");
	}
	server.on("/led_on", handleLedOn);
	server.on("/led_off", handleLedOff);
	server.onNotFound(handleNotFound);
	server.begin();
}

void calibrateStepper() {
	int count = 0;
	bool runLeft = true;
	while (runLeft) {
		stepper.move(4 * MICROSTEPS);
		count += 4 * MICROSTEPS;
		runLeft = !digitalRead(LEFT_STOP);
	}

	stepper.move(-count);

	bool runRight = true;
	while (runRight) {
		stepper.move(-4 * MICROSTEPS);
		count += 4 * MICROSTEPS;
		runRight = !digitalRead(RIGHT_STOP);
	}

	Serial.printf("I can count to %d!", count);
	stepper.move(count / 2);
}

void writeTilt(int deg) {
	servoTilt.write(constrain(deg - TILT_MIN, 0, TILT_LIMIT));
}

int prevTilt, nextTilt, currentTilt;
int prevPan, nextPan, currentPan;
int prevMove, nextMove, moveAmount;

int pos;

void setup() {
	/*
	 * Set target motor RPM.
	 */
	Serial.begin(9600);
	delay(20);
	stepper.begin(RPM, MICROSTEPS);
	// if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
	// stepper.setEnableActiveState(LOW);
	stepper.setSpeedProfile(stepper.LINEAR_SPEED, 6000, 6000);
	stepper.enable();
	servoTilt.setPeriodHertz(50);
	servoPan.setPeriodHertz(50);
	pinMode(LEFT_STOP, INPUT_PULLDOWN);
	pinMode(RIGHT_STOP, INPUT_PULLDOWN);
	pinMode(BUILTIN_FLASH, OUTPUT);
	servoTilt.attach(servoTiltPin, minUs, maxUs);
	delay(500);
	servoPan.attach(servoPanPin, minUs, maxUs);
	delay(500);

	pos = 90;
	writeTilt(pos);
	delay(500);
	servoPan.write(pos);
	delay(500);

	calibrateStepper();
	delay(200);
}

void loop() {
	/*
	 * We do a little jig both ways from the middle, resetting on the same place
	 */
	// delay(500);
	// stepper.rotate(360);
	// stepper.rotate(-2 * 360);
	// stepper.rotate(360);
	// servoTilt.writeMicroseconds(minUs);
	// servoTilt.write(90);
	// delay(5);
	// for (pos = TILT_MIN; pos <= TILT_MAX; pos += 1)
	
	for (; pos <= 180; pos += 1) {  // sweep from 0 degrees to 180 degrees
		// stepper.move(4 * MICROSTEPS);
		writeTilt(pos);
		servoPan.write(pos);
		// Serial.println(pos);
		delay(20);
		server.handleClient();
	}
	// for (pos = TILT_MAX; pos >= TILT_MIN; pos -= 1)
	for (pos = 180; pos >= 0; pos -= 1) {  // sweep from 180 degrees to 0 degrees
		// stepper.move(-4 * MICROSTEPS);
		writeTilt(pos);
		servoPan.write(pos);
		// Serial.println(pos);
		delay(20);
		server.handleClient();
	}
}