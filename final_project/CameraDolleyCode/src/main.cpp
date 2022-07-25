/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#define HW_TIMER_INTERVAL_US      20L
#define USING_PWM_FREQUENCY     true
#define USING_TIM_DIV1 true              // for shortest and most accurate timer

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <ESP8266_PWM.h>

#include <ArduinoJson.hpp>



const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

// Init ESP8266Timer
ESP8266Timer ITimer;

// Init ESP8266_ISR_PWM
ESP8266_PWM ISR_PWM;

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

void handle_Servo() {
    auto angle = server.arg("angle");
    String message = "Angle set: ";
    if (angle != "") {  
        ISR_PWM.setPWM(PWM_Pin, 20, calculateDutyCycle(angle.toInt()));
        message += angle.toInt();
    }
    message += "\n";
    server.send(200, "text/plain", message);
}

void IRAM_ATTR TimerHandler()
{
  ISR_PWM.run();
}

void setup() {
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
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

    if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
    {
      startMicros = micros();
      Serial.print(F("Starting ITimer OK, micros() = ")); 
      Serial.println(startMicros);
    }
  
    auto channel = ISR_PWM.setPWM(PWM_Pin, 20.0, 50.0);
    ISR_PWM.enable(channel);
    Serial.print("Pwm: "); Serial.println(channel);
    Serial.println("Setup done");
}

void loop() {
    server.handleClient();
    digitalWrite(LED_BUILTIN, LED1status);
}
