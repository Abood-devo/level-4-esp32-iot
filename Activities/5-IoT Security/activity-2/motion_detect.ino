// Pin definitions
#define PIR_PIN     13  // Motion Sensor
#define RED_LED     2   // Alert LED
#define GREEN_LED   42   // Status LED
#define BUZZER      15  // Buzzer
#define BUTTON_PIN  21  // Arm/Disarm Button

// System states
bool systemArmed = false;
bool motionDetected = false;
unsigned long lastDetectionTime = 0;
const unsigned long COOLDOWN_PERIOD = 5000; // 5 seconds cooldown

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(PIR_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initial system state
  digitalWrite(GREEN_LED, HIGH);
  startupSequence();
  Serial.println("Security System Ready!");
}

void loop() {
  // Check arm/disarm button
  if (digitalRead(BUTTON_PIN) == LOW) {
    toggleSystem();
    delay(500); // Debounce
  }
  
  if (systemArmed) {
    checkMotion();
  }
}

void startupSequence() {
  // Power-on test sequence
  digitalWrite(RED_LED, HIGH);
  beep(100);
  delay(200);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  beep(100);
  delay(200);
}

void toggleSystem() {
  systemArmed = !systemArmed;
  if (systemArmed) {
    Serial.println("System Armed!");
    digitalWrite(GREEN_LED, LOW);
    // Double beep for arming
    beep(100);
    delay(100);
    beep(100);
  } else {
    Serial.println("System Disarmed!");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    // Single long beep for disarming
    beep(500);
  }
}

void checkMotion() {
  if (digitalRead(PIR_PIN) == HIGH) {
    unsigned long currentTime = millis();
    // Check if enough time has passed since last detection
    if (!motionDetected && (currentTime - lastDetectionTime > COOLDOWN_PERIOD)) {
      motionDetected = true;
      lastDetectionTime = currentTime;
      triggerAlarm();
    }
  } else {
    motionDetected = false;
  }
}

void triggerAlarm() {
  Serial.println("Motion Detected!");
  for(int i = 0; i < 5; i++) {
    digitalWrite(RED_LED, HIGH);
    beep(200);
    digitalWrite(RED_LED, LOW);
    delay(200);
  }
}

void beep(int duration) {
  digitalWrite(BUZZER, HIGH);
  delay(duration);
  digitalWrite(BUZZER, LOW);
}