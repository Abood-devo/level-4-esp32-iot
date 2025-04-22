// Pin Definitions
#define LDR_PIN 36        // Light sensor analog pin
#define PIR_PIN 13        // Motion sensor digital pin
#define RELAY_PIN 12      // Relay control pin

// Thresholds and Timing
#define LIGHT_THRESHOLD 2000    // Adjust based on your room (0-4095)
#define MOTION_TIMEOUT 30000    // 30 seconds
#define RELAY_DELAY 1000       // Delay between relay switches (1 second)

// Variables
int lightLevel;
bool motionDetected = false;
bool relayState = false;
unsigned long lastMotionTime = 0;
unsigned long lastRelaySwitch = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Configure pins
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Initial relay state (OFF)
  digitalWrite(RELAY_PIN, LOW);
  
  // Setup message
  Serial.println("\nSmart Lighting System");
  Serial.println("-------------------");
  Serial.println("LDR Pin: 36");
  Serial.println("PIR Pin: 13");
  Serial.println("Relay Pin: 12");
  Serial.println("System Initialized!\n");
}

void loop() {
  // Read sensor values
  lightLevel = analogRead(LDR_PIN);
  motionDetected = digitalRead(PIR_PIN);
  
  // Update motion timer if motion detected
  if (motionDetected) {
    lastMotionTime = millis();
  }
  
  // Print sensor values
  printSensorValues();
  
  // Check if conditions are met to change relay state
  bool shouldBeOn = shouldActivateRelay();
  
  // Control relay with delay protection
  if (shouldBeOn != relayState) {
    if (millis() - lastRelaySwitch >= RELAY_DELAY) {
      controlRelay(shouldBeOn);
      lastRelaySwitch = millis();
    }
  }
  
  // Small delay to prevent serial flooding
  delay(500);
}

bool shouldActivateRelay() {
  // Check if it's dark enough (LDR reading above threshold)
  bool isDark = (lightLevel > LIGHT_THRESHOLD);
  
  // Check if motion was detected recently
  bool recentMotion = (millis() - lastMotionTime < MOTION_TIMEOUT);
  
  // Return true if both conditions are met
  return isDark && recentMotion;
}

void controlRelay(bool turnOn) {
  digitalWrite(RELAY_PIN, turnOn);
  relayState = turnOn;
  
  Serial.print("\n→ Relay turned ");
  Serial.println(turnOn ? "ON" : "OFF");
  Serial.println("-------------------");
}

void printSensorValues() {
  Serial.println("Sensor Readings:");
  Serial.printf("Light Level: %d ", lightLevel);
  Serial.println(lightLevel > LIGHT_THRESHOLD ? "(Dark)" : "(Bright)");
  Serial.printf("Motion: %s\n", motionDetected ? "Detected" : "None");
  Serial.printf("Time since last motion: %d ms\n", 
                (int)(millis() - lastMotionTime));
}