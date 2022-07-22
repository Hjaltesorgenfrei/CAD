#include <Arduino.h>

#define LEFT_MOTOR_OUT 3
#define RIGHT_MOTOR_OUT 6
#define LEFT_SENSOR_IN 2
#define RIGHT_SENSOR_IN 4
#define LED_PIN 13
#define MAX_SPEED 220 // 255 is max

#define RIGHT true;
#define LEFT false;

bool direction;

void setup()
{
  pinMode(LEFT_MOTOR_OUT, OUTPUT);
  pinMode(RIGHT_MOTOR_OUT, OUTPUT);
  pinMode(LEFT_SENSOR_IN, INPUT);
  pinMode(RIGHT_SENSOR_IN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  direction = RIGHT;
}

void loop()
{
  bool leftNotOver = digitalRead(LEFT_SENSOR_IN);
  bool rightNotOver = digitalRead(RIGHT_SENSOR_IN);

  if (direction)
  {
    if (leftNotOver)
    {
      analogWrite(LEFT_MOTOR_OUT, MAX_SPEED);
    }
    else
    {
      analogWrite(LEFT_MOTOR_OUT, 0);
      direction = LEFT;
    }
  }
  else
  {
    if (rightNotOver)
    {
      analogWrite(RIGHT_MOTOR_OUT, MAX_SPEED);
    }
    else
    {
      analogWrite(RIGHT_MOTOR_OUT, 0);
      direction = RIGHT;
    }
  }

  if (direction) // Light if direction set to right
  {
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }
}
