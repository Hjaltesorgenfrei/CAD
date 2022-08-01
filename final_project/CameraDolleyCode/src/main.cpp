/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#define HW_TIMER_INTERVAL_US 20L
#define USING_PWM_FREQUENCY true
#define USING_TIM_DIV1 true  // for shortest and most accurate timer

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include <ArduinoJson.hpp>

const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

// IPAddress local_ip(192, 168, 4, 1);
// IPAddress gateway(192, 168, 4, 1);
// IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

int LED1status = LOW;

uint32_t PWM_Pin = D4;
volatile uint32_t startMicros = 0;

void handle_led1on() {
    LED1status = HIGH;
    Serial.print("led on");
    server.send(200, "text/plain", "Led on");
}

void handle_led1off() {
    LED1status = LOW;
    Serial.print("led off");
    server.send(200, "text/plain", "Led off");
}

void handle_NotFound() {
    server.send(404, "text/plain", "Not found");
}

void handle_Move() {
    if (server.hasArg("plain") == false) {  // Check if body received
        server.send(200, "text/plain", "Body not received");
        return;
    }

    String message = "Body received:\n";
    message += server.arg("plain");
    message += "\n";

    server.send(200, "text/plain", message);
    Serial.println(message);
}

float calculateDutyCycle(int angle) {
    return 5.0 + ((angle / 180.0) * 5.0);
}

#define TIMER_INTERRUPT_DEBUG 1
#define ISR_SERVO_DEBUG 0

#include "ESP8266_ISR_Servo.h"

#define MIN_MICROS 500  // 544
#define MAX_MICROS 2500

uint64_t duty_micros = 1500;
int position = 0;
uint8_t servo_index = 0;

void handle_Servo() {
    auto angle = server.arg("angle");
    String message = "Angle set: ";
    if (angle != "") {
        duty_micros = map(angle.toInt(), 0, 180, 1000, 2000);
        ISR_Servo.setPosition(servo_index, position);
        message += angle.toInt();
        position = angle.toInt();
    }
    message += "\n";
    server.send(200, "text/plain", message);
}

#define STEP_PIN D1
#define DIR_PIN D2

void setup() {
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    Serial.begin(9600);

    boolean result = WiFi.softAP(ssid, password, 13);
    // WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);
    if (result) {
        Serial.println("Ready");
    } else {
        Serial.println("Failed!");
    }

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.on("/led1on", handle_led1on);
    server.on("/led1off", handle_led1off);
    server.on("/move", HTTP_POST, handle_Move);
    server.on("/servo_test", HTTP_POST, handle_Servo);
    server.onNotFound(handle_NotFound);
    server.begin();

    servo_index = ISR_Servo.setupServo(D2, MIN_MICROS, MAX_MICROS);
    ISR_Servo.setPosition(servo_index, position);

    Serial.println("Setup done");
}

uint64_t period = 0;
boolean servo_on = false;

void loop() {
    server.handleClient();
    digitalWrite(LED_BUILTIN, LED1status);
    digitalWrite(DIR_PIN, HIGH);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP_PIN, LOW);  
}
