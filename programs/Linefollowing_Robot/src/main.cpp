#include <Arduino.h>

#define LEFT_MOTOR_OUT 3
#define RIGHT_MOTOR_OUT 6
#define LEFT_SENSOR_IN 2
#define RIGHT_SENSOR_IN 4
#define LED_PIN 13
#define MAX_SPEED 255
#define RIGHT_SPEED 240
#define LEFT_SPEED 210

#define RIGHT true;
#define LEFT false;

#define MAX_SPEED_TIME 200

int liveTime;
bool direction;

void setup()
{
  pinMode(LEFT_MOTOR_OUT, OUTPUT);
  pinMode(RIGHT_MOTOR_OUT, OUTPUT);
  pinMode(LEFT_SENSOR_IN, INPUT);
  pinMode(RIGHT_SENSOR_IN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  direction = RIGHT;
  liveTime = 0;
}

void loop()
{
  bool leftNotOver = digitalRead(LEFT_SENSOR_IN);
  bool rightNotOver = digitalRead(RIGHT_SENSOR_IN);

  liveTime++;

  // if (!leftNotOver && !rightNotOver)
  // {
  //   analogWrite(LEFT_MOTOR_OUT, liveTime < MAX_SPEED_TIME ? MAX_SPEED : LEFT_SPEED);
  //   analogWrite(RIGHT_MOTOR_OUT, liveTime < MAX_SPEED_TIME ? MAX_SPEED : RIGHT_SPEED);
  //   return;
  // }

  if (direction)
  {
    if (leftNotOver)
    {
      analogWrite(LEFT_MOTOR_OUT, liveTime < MAX_SPEED_TIME ? MAX_SPEED : LEFT_SPEED);
    }
    else
    {
      analogWrite(LEFT_MOTOR_OUT, 0);
      analogWrite(RIGHT_MOTOR_OUT, MAX_SPEED); // Initial burst is max speed for better acceleration
      direction = LEFT;
      digitalWrite(LED_PIN, LOW);
      liveTime = 0;
    }
  }
  else
  {
    if (rightNotOver)
    {
      analogWrite(RIGHT_MOTOR_OUT, liveTime < MAX_SPEED_TIME ? MAX_SPEED : RIGHT_SPEED);
    }
    else
    {
      analogWrite(RIGHT_MOTOR_OUT, 0);
      analogWrite(LEFT_MOTOR_OUT, MAX_SPEED);

      direction = RIGHT;
      digitalWrite(LED_PIN, HIGH);
      liveTime = 0;
    }
  }
}
