#define BLYNK_TEMPLATE_ID "TMPL62RGEpVN9"
#define BLYNK_TEMPLATE_NAME "RGB LED Control"
#define BLYNK_AUTH_TOKEN "E7jVS0MdVPY_da5t5CGDJ6BZsQ1M5YiM"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "esp32-hal-ledc.h"

// WiFi credentials
const char* ssid = "ESP-IOT-WIFI";
const char* password = "12345678";

// LED pins
const int redPin = 42;
const int greenPin = 40;
const int bluePin = 38;

// PWM properties
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  
  // Configure LED PWM with new API
  ledcAttach(redPin, freq, resolution);
  ledcAttach(greenPin, freq, resolution);
  ledcAttach(bluePin, freq, resolution);
  
  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop() {
  Blynk.run();
}

// Blynk virtual pin handlers
BLYNK_WRITE(V0) {  // Red slider
  int value = param.asInt();
  ledcWrite(redPin, value);
}

BLYNK_WRITE(V1) {  // Green slider
  int value = param.asInt();
  ledcWrite(greenPin, value);
}

BLYNK_WRITE(V2) {  // Blue slider
  int value = param.asInt();
  ledcWrite(bluePin, value);
}

BLYNK_WRITE(V3) {  // Color picker
  int r = param[0].asInt();
  int g = param[1].asInt();
  int b = param[2].asInt();
  
  ledcWrite(redPin, r);
  ledcWrite(greenPin, g);
  ledcWrite(bluePin, b);
}