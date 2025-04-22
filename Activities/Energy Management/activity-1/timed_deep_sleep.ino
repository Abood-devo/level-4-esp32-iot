#include <DHT.h>

// Pin Definitions
#define DHTPIN 4
#define LED_PIN 2

// Deep Sleep Parameters
#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  30       // Time ESP32 will go to sleep (in seconds)

// Temperature Thresholds
#define TEMP_HIGH 28
#define TEMP_LOW 20

// Initialize DHT sensor
DHT dht(DHTPIN, DHT11);

// Variables to store in RTC memory (survives deep sleep)
RTC_DATA_ATTR int bootCount = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
  
  // Increment boot count
  bootCount++;
  
  // Print boot count
  Serial.println("Boot number: " + String(bootCount));

  // Configure deep sleep wake up timer
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  
  // Perform temperature reading and indication
  performTemperatureCheck();
  
  // Go to sleep
  prepareSleep();
}

void loop() {
  // This will never run as device goes to sleep in setup
}

void performTemperatureCheck() {
  // Read temperature
  float temperature = dht.readTemperature();
  
  // Check if reading is valid
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    errorBlink();
    return;
  }
  
  // Print temperature
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  
  // Indicate temperature with LED
  if (temperature > TEMP_HIGH) {
    highTempBlink();
  } else if (temperature < TEMP_LOW) {
    lowTempBlink();
  } else {
    normalTempBlink();
  }
}

void highTempBlink() {
  // Rapid blinks for high temperature
  for(int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void lowTempBlink() {
  // Slow blinks for low temperature
  for(int i = 0; i < 2; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

void normalTempBlink() {
  // Single blink for normal temperature
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}

void errorBlink() {
  // Error indication
  for(int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}

void prepareSleep() {
  Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) + " seconds");
  Serial.flush(); 
  esp_deep_sleep_start();
}