#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

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

#define DIR_PIN 13
#define STEP_PIN 12

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
A4988 stepper(MOTOR_STEPS, DIR_PIN, STEP_PIN);

int moveSteps = 0; // Initialize to the movement we do back froscehamticm the wall

const char *ssid = "ESP_Cam";
const char *password = "29292929";

WebServer server(80);

boolean LEDStatus = LOW;

void handleMoveForm()
{
	server.send(200, "text/html", "<!DOCTYPE html>\
<html>\
  <head>\
    <title>Move that thang</title>\
    <style>\
      div { padding: 4px; }\
      label { padding-right: 4px; }\
    </style>\
  </head>\
  <body>\
    <form action=\"move\" method=\"GET\">\
      <div><label for=\"pan\">Pan</label><input id=\"pan\" name=\"pan\" type=\"range\" min=\"0\" max=\"180\" /></div>\
      <div><label for=\"tilt\">Tilt</label><input id=\"tilt\" name=\"tilt\" type=\"range\" min=\"" +
																		(String(TILT_MIN)) + "\" max=\"" + (String(TILT_MAX)) + "\" /></div>\
      <div><label for=\"move\">Move</label><input id=\"move\" name=\"move\" type=\"range\" min=\"0\" max=\"" +
																		(String(moveSteps)) + "\" value=\"" + (String(moveSteps)) + " / 2\" /></div>\
      <input type=\"submit\" />\
    </form>\
  </body>\
</html>");
}

void handleLedOn()
{
	LEDStatus = HIGH;
	digitalWrite(4, LEDStatus);
	server.send(200, "text/plain", "Led On");
}

void handleLedOff()
{
	LEDStatus = LOW;
	digitalWrite(4, LEDStatus);
	server.send(200, "text/html", "Led On");
}

void handleNotFound()
{
	server.send(404, "text/html", "Not Found");
}

void setupWebServer()
{
	boolean result = WiFi.softAP(ssid, password, 13);
	if (result)
	{
		Serial.println("Ready");
	}
	else
	{
		Serial.println("Failed!");
	}
	server.on("/led_on", handleLedOn);
	server.on("/led_off", handleLedOff);
	server.on("/", handleMoveForm);
	server.onNotFound(handleNotFound);
	server.begin();
}

bool leftTriggered = false;
bool rightTriggered = false;

void calibrateStepper()
{
	while (!leftTriggered)
	{
		stepper.move(4 * MICROSTEPS);
		moveSteps += 4 * MICROSTEPS;
		delay(1);
	}

	stepper.move(-moveSteps);

	while (!rightTriggered)
	{
		stepper.move(-4 * MICROSTEPS);
		moveSteps += 4 * MICROSTEPS;
		delay(1);
	}

	Serial.printf("I can count to %d!\n", moveSteps);
	stepper.move(moveSteps / 2);
}

void writeTilt(int deg)
{
	servoTilt.write(constrain(deg - TILT_MIN, 0, TILT_LIMIT));
}

int prevTilt, nextTilt, currentTilt;
int prevPan, nextPan, currentPan;
int prevMove, nextMove, currentMove, moveDiff;

int currentStep;
#define MAX_STEPS 1000
#define STEP 10

void setNextPosition(int tilt, int pan, int move)
{
	prevTilt = currentTilt;
	prevPan = currentPan;
	prevMove = currentMove;
	nextTilt = tilt;
	nextPan = pan;
	nextMove = move;
	currentStep = 0;
}

int pos;

void IRAM_ATTR endStopIsr()
{
	leftTriggered = digitalRead(LEFT_STOP);
	rightTriggered = digitalRead(RIGHT_STOP);
}

void setup()
{
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
	/*
	 * Set target motor RPM.
	 */
	Serial.begin(9600);
	delay(1500);
	setupWebServer();
	delay(500);
	servoTilt.setPeriodHertz(50);
	servoPan.setPeriodHertz(50);
	pinMode(LEFT_STOP, INPUT_PULLDOWN);
	attachInterrupt(LEFT_STOP, endStopIsr, CHANGE);
	leftTriggered = digitalRead(LEFT_STOP);
	pinMode(RIGHT_STOP, INPUT_PULLDOWN);
	attachInterrupt(RIGHT_STOP, endStopIsr, CHANGE);
	rightTriggered = digitalRead(RIGHT_STOP);
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

	stepper.begin(RPM, MICROSTEPS);
	// if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
	// stepper.setEnableActiveState(LOW);
	stepper.setSpeedProfile(stepper.LINEAR_SPEED, 6000, 6000);
	stepper.enable();
	delay(200);

	calibrateStepper();
	delay(200);
	currentTilt = 90;
	currentPan = 90;
	currentMove = moveSteps / 2;
	setNextPosition(150, 40, moveSteps * 4 / 5);
	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);
}

void interpolate()
{
	moveDiff = currentMove;
	currentMove = prevMove + (((nextMove - prevMove) * currentStep) / MAX_STEPS);
	moveDiff -= currentMove;
	currentPan = prevPan + (((nextPan - prevPan) * currentStep) / MAX_STEPS);
	currentTilt = prevTilt + (((nextTilt - prevTilt) * currentStep) / MAX_STEPS);
}

void loop()
{
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

	server.handleClient();

	currentStep += STEP;
	interpolate();
	if (currentStep >= MAX_STEPS)
	{
		currentStep = 0;
		setNextPosition(prevTilt, prevPan, prevMove);
		delay(10);
	}
	else
	{
		writeTilt(currentTilt);
		delay(2);
		servoPan.write(currentPan);
		delay(2);
		stepper.move(moveDiff);
		// Serial.printf("t: %d p: %d m: %d\n", currentTilt, currentPan, currentMove);
		delay(6);
	}

	// for (; pos <= 180; pos += 1)
	// { // sweep from 0 degrees to 180 degrees
	// 	// stepper.move(4 * MICROSTEPS);
	// 	writeTilt(pos);
	// 	servoPan.write(pos);
	// 	// Serial.println(pos);
	// 	delay(20);
	// 	server.handleClient();
	// }
	// // for (pos = TILT_MAX; pos >= TILT_MIN; pos -= 1)
	// for (pos = 180; pos >= 0; pos -= 1)
	// { // sweep from 180 degrees to 0 degrees
	// 	// stepper.move(-4 * MICROSTEPS);
	// 	writeTilt(pos);
	// 	servoPan.write(pos);
	// 	// Serial.println(pos);
	// 	delay(20);
	// 	server.handleClient();
	// }
}