#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "ESP-IOT-WIFI";
const char* password = "12345678";

// Define LED pins
const int redLED = 4;
const int greenLED = 5;
const int blueLED = 6;

// Create web server object
WebServer server(80);

// HTML webpage with buttons and styling
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 LED Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { 
            font-family: Arial; 
            text-align: center; 
            margin-top: 50px;
        }
        .button {
            padding: 15px 30px;
            font-size: 20px;
            margin: 10px;
            cursor: pointer;
            border-radius: 10px;
            border: none;
            color: white;
        }
        .red { background-color: #ff0000; }
        .green { background-color: #00ff00; }
        .blue { background-color: #0000ff; }
        .black { background-color: #000000; }
    </style>
</head>
<body>
    <h1>ESP32 LED Control</h1>
    <button class="button red" onclick="toggleLED('red')">Red LED</button><br>
    <button class="button green" onclick="toggleLED('green')">Green LED</button><br>
    <button class="button blue" onclick="toggleLED('blue')">Blue LED</button><br>
    <button class="button black" onclick="toggleLED('all')">Toggle All</button>
    
    <script>
        function toggleLED(color) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/" + color, true);
            xhr.send();
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Set LED pins as outputs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Define server endpoints
  server.on("/", HTTP_GET, handleRoot);
  server.on("/red", HTTP_GET, handleRed);
  server.on("/green", HTTP_GET, handleGreen);
  server.on("/blue", HTTP_GET, handleBlue);
  server.on("/all", HTTP_GET, handleAll);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleRed() {
  digitalWrite(redLED, !digitalRead(redLED));
  server.send(200, "text/plain", "Red LED toggled");
  Serial.println("Red LED toggled");
}

void handleGreen() {
  digitalWrite(greenLED, !digitalRead(greenLED));
  server.send(200, "text/plain", "Green LED toggled");
  Serial.println("Green LED toggled");
}

void handleBlue() {
  digitalWrite(blueLED, !digitalRead(blueLED));
  server.send(200, "text/plain", "Blue LED toggled");
  Serial.println("Blue LED toggled");
}

void handleAll() {
  bool state = digitalRead(redLED) || digitalRead(greenLED) || digitalRead(blueLED);
  digitalWrite(redLED, !state);
  digitalWrite(greenLED, !state);
  digitalWrite(blueLED, !state);
  server.send(200, "text/plain", "All LEDs toggled");
  Serial.println("All LEDs toggled");
}