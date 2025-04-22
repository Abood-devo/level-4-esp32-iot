#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_camera.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Gemini API settings
const char* apiKey = "YOUR_API_KEY";
const char* apiEndpoint = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent";

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Camera pins for AI Thinker
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  Serial.begin(115200);
  Wire.begin(14, 15); // SDA, SCL

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Starting up...");
  display.display();

  // Initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Initialize with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  if(digitalRead(13) == LOW) { // Button pressed
    captureAndAnalyze();
    delay(1000);
  }
}

void captureAndAnalyze() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Capturing image...");
  display.display();

  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Convert image to base64
  String base64Image = base64::encode(fb->buf, fb->len);
  esp_camera_fb_return(fb);

  // Prepare API request
  HTTPClient http;
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-goog-api-key", apiKey);

  // Create JSON payload
  DynamicJsonDocument doc(20000);
  JsonArray contents = doc.createNestedArray("contents");
  JsonObject content = contents.createNestedObject();
  JsonArray parts = content.createNestedArray("parts");
  
  JsonObject textPart = parts.createNestedObject();
  textPart["text"] = "Describe this image";

  JsonObject imagePart = parts.createNestedObject();
  JsonObject inlineData = imagePart.createNestedObject("inlineData");
  inlineData["mimeType"] = "image/jpeg";
  inlineData["data"] = base64Image;

  String jsonString;
  serializeJson(doc, jsonString);

  // Send request
  int httpResponseCode = http.POST(jsonString);

  if(httpResponseCode > 0) {
    String response = http.getString();
    
    // Parse response
    DynamicJsonDocument responseDoc(2048);
    deserializeJson(responseDoc, response);
    
    // Extract description
    String description = responseDoc["candidates"][0]["content"]["parts"][0]["text"].as<String>();

    // Display description
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("AI sees:");
    display.println();
    
    // Word wrap the description
    int16_t x1, y1;
    uint16_t w, h;
    String currentLine = "";
    String words = description;
    
    while(words.length() > 0) {
      int spaceIndex = words.indexOf(' ');
      String word = (spaceIndex == -1) ? words : words.substring(0, spaceIndex);
      
      display.getTextBounds(currentLine + word, 0, 0, &x1, &y1, &w, &h);
      if(w > SCREEN_WIDTH) {
        display.println(currentLine);
        currentLine = word + " ";
      } else {
        currentLine += word + " ";
      }
      
      if(spaceIndex == -1) {
        display.println(currentLine);
        break;
      }
      words = words.substring(spaceIndex + 1);
    }
    
    display.display();
  }
  
  http.end();
}