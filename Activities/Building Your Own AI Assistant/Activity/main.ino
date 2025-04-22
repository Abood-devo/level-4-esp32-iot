#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/i2s.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// API Keys
const char* DEEPGRAM_API_KEY = "YOUR_DEEPGRAM_API_KEY";
const char* GEMINI_API_KEY = "YOUR_GEMINI_API_KEY";

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2S Microphone settings
#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0
#define BUFFER_SIZE 1024
int16_t sBuffer[BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("ESP32 AI Assistant");
  display.display();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Configure I2S
  esp_err_t err;
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.println("Failed to install I2S driver");
    return;
  }

  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.println("Failed to set I2S pins");
    return;
  }
}

String recordAudio() {
  size_t bytesRead = 0;
  String audioData = "";
  
  // Record for 5 seconds
  for(int i = 0; i < 160; i++) {
    i2s_read(I2S_PORT, sBuffer, BUFFER_SIZE * 2, &bytesRead, portMAX_DELAY);
    for(int j = 0; j < BUFFER_SIZE; j++) {
      audioData += String(sBuffer[j]) + ",";
    }
  }
  return audioData;
}

String speechToText(String audioData) {
  HTTPClient http;
  http.begin("https://api.deepgram.com/v1/listen");
  http.addHeader("Authorization", "Token " + String(DEEPGRAM_API_KEY));
  http.addHeader("Content-Type", "audio/raw");
  
  int httpCode = http.POST(audioData);
  String response = http.getString();
  http.end();

  // Parse JSON response
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  return doc["results"]["channels"][0]["alternatives"][0]["transcript"].as<String>();
}

String getGeminiResponse(String query) {
  HTTPClient http;
  http.begin("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(GEMINI_API_KEY));
  http.addHeader("Content-Type", "application/json");
  
  String payload = "{\"contents\":[{\"parts\":[{\"text\":\"" + query + "\"}]}]}";
  int httpCode = http.POST(payload);
  String response = http.getString();
  http.end();

  // Parse JSON response
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  return doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
}

void displayResponse(String text) {
  display.clearDisplay();
  display.setCursor(0,0);
  
  // Simple text wrapping
  int16_t x1, y1;
  uint16_t w, h;
  String currentLine = "";
  int currentY = 0;
  
  for(unsigned int i = 0; i < text.length(); i++) {
    currentLine += text[i];
    display.getTextBounds(currentLine, 0, 0, &x1, &y1, &w, &h);
    
    if(w > SCREEN_WIDTH || text[i] == '\n') {
      display.setCursor(0, currentY);
      display.println(currentLine);
      currentLine = "";
      currentY += 8;
      
      if(currentY > SCREEN_HEIGHT) {
        display.display();
        delay(2000);
        display.clearDisplay();
        currentY = 0;
      }
    }
  }
  
  if(currentLine.length() > 0) {
    display.setCursor(0, currentY);
    display.println(currentLine);
  }
  
  display.display();
}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Listening...");
  display.display();

  String audioData = recordAudio();
  String text = speechToText(audioData);
  
  if(text.length() > 0) {
    String response = getGeminiResponse(text);
    displayResponse(response);
  }
  
  delay(1000);
}