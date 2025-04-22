#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>

// Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3c // Typical I2C address for this OLED

// Initlizing i2c pins
#define SDA_PIN 21 // Typical I2C address for this OLED
#define SCL_PIN 22 // Typical I2C address for this OLED

// Create objects
Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize BME280
  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found!");
    while (1);
  }

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Display not found!");
    while(1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  // Read sensor data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  // Display data
  display.clearDisplay();
  display.setCursor(0,0);
  
  display.println("Weather Station");
  display.println("-------------");
  
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");
  
  display.print("Humidity: ");
  display.print(humidity, 0);
  display.println(" %");
  
  display.print("Pressure: ");
  display.print(pressure, 0);
  display.println(" hPa");
  
  display.display();
  
  // Also print to Serial Monitor
  Serial.println("\nWeather Readings:");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
  
  delay(2000);
}