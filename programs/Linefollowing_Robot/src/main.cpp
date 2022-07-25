#include <Arduino.h>

#define LEFT_MOTOR_OUT 3
#define RIGHT_MOTOR_OUT 6
#define LEFT_SENSOR_IN 2
#define RIGHT_SENSOR_IN 4
#define LED_PIN 13
#define RIGHT_SPEED 200
#define LEFT_SPEED 170

#define RIGHT true;
#define LEFT false;

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

  if (!leftNotOver && !rightNotOver)
  {
    analogWrite(LEFT_MOTOR_OUT, LEFT_SPEED);
    analogWrite(RIGHT_MOTOR_OUT, RIGHT_SPEED);
  }
  else if (direction)
  {
    if (leftNotOver)
    {
      analogWrite(LEFT_MOTOR_OUT, LEFT_SPEED);
      analogWrite(RIGHT_MOTOR_OUT, 0);
    }
    else
    {
      analogWrite(LEFT_MOTOR_OUT, 0);
      analogWrite(RIGHT_MOTOR_OUT, RIGHT_SPEED); // Initial burst is max speed for better acceleration
      direction = LEFT;
      digitalWrite(LED_PIN, LOW);
      liveTime = 0;
    }
  }
  else
  {
    if (rightNotOver)
    {
      analogWrite(RIGHT_MOTOR_OUT, RIGHT_SPEED);
      analogWrite(LEFT_MOTOR_OUT, 0);
    }
    else
    {
      analogWrite(RIGHT_MOTOR_OUT, 0);
      analogWrite(LEFT_MOTOR_OUT, LEFT_SPEED);

      direction = RIGHT;
      digitalWrite(LED_PIN, HIGH);
      liveTime = 0;
    }
  }
}
