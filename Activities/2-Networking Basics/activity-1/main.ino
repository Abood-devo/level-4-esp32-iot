#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Define custom pins for I2C communication
#define SDA_PIN 45
#define SCL_PIN 48

// WiFi credentials
const char* ssid = "myWeather";
const char* password = "12345678";

// OpenWeatherMap settings
const String apiKey = "API_KEY";  // This will be provided by instructor
const String city = "Amman";          // Change to your city
const String country = "JO";           // Change to your country code

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Start Serial for debugging
  Serial.begin(115200);
  
  // Initialize the Wire library with custom pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize the LCD
  lcd.init();
  
  // Turn on the backlight
  lcd.backlight();
  
  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Get and display weather every 5 minutes
  getAndDisplayWeather();
  delay(5 * 60); // Wait 5 minutes
}

void connectToWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi..");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  lcd.clear();
  lcd.print("WiFi Connected!");
  delay(2000);
}

void getAndDisplayWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Create the URL for the OpenWeatherMap API request
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + 
                 city + "," + country + 
                 "&APPID=" + apiKey + 
                 "&units=metric"; // Use metric units
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String payload = http.getString();
      
      // Use ArduinoJson to parse the response
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      
      // Get the weather data
      float temp = doc["main"]["temp"];
      int humidity = doc["main"]["humidity"];
      String weather = doc["weather"][0]["main"].as<String>();
      
      // Display on LCD
      lcd.clear();
      
      // First row: Temperature and humidity
      lcd.setCursor(0, 0);
      lcd.print(temp, 1);
      lcd.print("C ");
      lcd.print(humidity);
      lcd.print("%");
      
      // Second row: Weather condition
      lcd.setCursor(0, 1);
      lcd.print(weather);
      
      // Debug output
      Serial.print("Temperature: ");
      Serial.println(temp);
      Serial.print("Humidity: ");
      Serial.println(humidity);
      Serial.print("Weather: ");
      Serial.println(weather);
    }
    else {
      lcd.clear();
      lcd.print("Error getting");
      lcd.setCursor(0, 1);
      lcd.print("weather data");
      Serial.print("Weather: ");
    }
    
    http.end();
  }
}