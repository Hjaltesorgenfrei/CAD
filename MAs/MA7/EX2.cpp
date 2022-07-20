#include <Arduino.h>

#define LED_PIN 13
#define SWITCH_PIN 3

bool LED_ON = false;

void blink() {
    LED_ON = !LED_ON;
    digitalWrite(LED_PIN, LED_ON);
}

void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_PIN, OUTPUT);
    pinMode(SWITCH_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), blink, FALLING);
}

void loop() {

}

