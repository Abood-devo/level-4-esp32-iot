#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Network credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram BOT Token (from BotFather)
#define BOTtoken "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

// Pin definitions
#define LED_PIN 2
#define DHTPIN 4
#define DHTTYPE DHT11   // Change to DHT22 if using that sensor

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize Telegram bot
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
  
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  
  // Send welcome message
  String welcome = "Bot started up!\n\n";
  welcome += "Available commands:\n";
  welcome += "/led_on : Turn LED on\n";
  welcome += "/led_off : Turn LED off\n";
  welcome += "/status : Get LED status\n";
  welcome += "/temperature : Get temperature\n";
  welcome += "/humidity : Get humidity\n";
  bot.sendMessage(CHAT_ID, welcome, "");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    
    if (text == "/led_on") {
      digitalWrite(LED_PIN, HIGH);
      bot.sendMessage(chat_id, "LED is ON", "");
    }
    
    else if (text == "/led_off") {
      digitalWrite(LED_PIN, LOW);
      bot.sendMessage(chat_id, "LED is OFF", "");
    }
    
    else if (text == "/status") {
      String status = "LED is ";
      status += (digitalRead(LED_PIN) ? "ON" : "OFF");
      bot.sendMessage(chat_id, status, "");
    }
    
    else if (text == "/temperature") {
      float temperature = dht.readTemperature();
      if (isnan(temperature)) {
        bot.sendMessage(chat_id, "Failed to read temperature!", "");
      } else {
        String temp = "Temperature: ";
        temp += String(temperature, 1);
        temp += "°C";
        bot.sendMessage(chat_id, temp, "");
      }
    }
    
    else if (text == "/humidity") {
      float humidity = dht.readHumidity();
      if (isnan(humidity)) {
        bot.sendMessage(chat_id, "Failed to read humidity!", "");
      } else {
        String hum = "Humidity: ";
        hum += String(humidity, 1);
        hum += "%";
        bot.sendMessage(chat_id, hum, "");
      }
    }
    
    else if (text == "/start") {
      String welcome = "Welcome to ESP32 Control Bot!\n\n";
      welcome += "Available commands:\n";
      welcome += "/led_on : Turn LED on\n";
      welcome += "/led_off : Turn LED off\n";
      welcome += "/status : Get LED status\n";
      welcome += "/temperature : Get temperature\n";
      welcome += "/humidity : Get humidity\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    else {
      String message = "Command not recognized.\n";
      message += "Use /start to see available commands.";
      bot.sendMessage(chat_id, message, "");
    }
  }
}