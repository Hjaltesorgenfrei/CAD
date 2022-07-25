/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <ArduinoJson.hpp>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

int LED1status = LOW;

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
    server.onNotFound(handle_NotFound);
    server.begin();

    Serial.println("Setup done");
}

void loop() {
    server.handleClient();
    digitalWrite(LED_BUILTIN, LED1status);
}
