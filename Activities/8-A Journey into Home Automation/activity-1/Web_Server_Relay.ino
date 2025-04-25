#include "WiFi.h"
#include "ESPAsyncWebServer.h"

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO    true

// Define the relay GPIO pin
const int relayPin = 2;  // Using GPIO 2, change this to match your connection

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

const char* PARAM_INPUT_1 = "relay";  
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay=1&state=1", true); }
  else { xhr.open("GET", "/update?relay=1&state=0", true); }
  xhr.send();
}</script>
</body>
</html>
)rawliteral";

String processor(const String& var){
  if(var == "BUTTONPLACEHOLDER"){
    String relayStateValue = relayState();
    String button = "<h4>Relay - GPIO " + String(relayPin) + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    return button;
  }
  return String();
}

String relayState(){
  if(RELAY_NO){
    return digitalRead(relayPin) ? "" : "checked";
  }
  else {
    return digitalRead(relayPin) ? "checked" : "";
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Set relay pin as output
  pinMode(relayPin, OUTPUT);
  
  // Set relay to off when the program starts
  if(RELAY_NO){
    digitalWrite(relayPin, HIGH);
  }
  else{
    digitalWrite(relayPin, LOW);
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?relay=1&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage2;
    // GET state value
    if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      if(RELAY_NO){
        digitalWrite(relayPin, !inputMessage2.toInt());
      }
      else{
        digitalWrite(relayPin, inputMessage2.toInt());
      }
    }
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {
  // No need for anything in the loop for this application
}