#include <Arduino.h>

#define PWM_PIN 5


void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(PWM_PIN, OUTPUT);
}

void loop() {
  int val = analogRead(A5);
  analogWrite(PWM_PIN, val / 2);
}

