#include <Arduino.h>

#define LEFT_MOTOR_OUT 3
#define RIGHT_MOTOR_OUT 6
#define LEFT_SENSOR_IN 2
#define RIGHT_SENSOR_IN 4
#define LED_PIN 13
#define MAX_SPEED 210 //255 is max

void setup() {
    pinMode(LEFT_MOTOR_OUT, OUTPUT);
    pinMode(RIGHT_MOTOR_OUT, OUTPUT);
    pinMode(LEFT_SENSOR_IN, INPUT);
    pinMode(RIGHT_SENSOR_IN, INPUT);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
  bool leftNotOver = digitalRead(LEFT_SENSOR_IN);
  bool rightNotOver = digitalRead(RIGHT_SENSOR_IN);

  if(leftNotOver) {
    analogWrite(LEFT_MOTOR_OUT, MAX_SPEED);
  }
  else {
    analogWrite(LEFT_MOTOR_OUT, 0);
  }
  
  if(rightNotOver) {
    analogWrite(RIGHT_MOTOR_OUT, MAX_SPEED);
  }
  else {
    analogWrite(RIGHT_MOTOR_OUT, 0);
  }

  if (!rightNotOver) {
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
}