#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "ESP-IOT-WIFI";
const char* password = "12345678";

// ThingSpeak settings
String writeAPIKey = "YOUR_WRITE_API_KEY";

// DHT sensor settings
#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Print values to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  // Send data to ThingSpeak
  sendToThingSpeak(temperature, humidity);
  
  // Wait for 15 seconds (ThingSpeak needs at least 15 seconds between updates)
  delay(15000);
}

void sendToThingSpeak(float temp, float hum) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Prepare URL
    String url = "http://api.thingspeak.com/update?api_key=";
    url += writeAPIKey;
    url += "&field1=";
    url += String(temp);
    url += "&field2=";
    url += String(hum);
    
    // Send HTTP GET request
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully!");
    } else {
      Serial.print("Error sending data. Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
}