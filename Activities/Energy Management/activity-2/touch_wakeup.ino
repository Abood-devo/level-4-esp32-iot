// Pin Definitions
#define TOUCH_PIN 15    // Touch sensor pin
#define LED_PIN 2       // LED pin
#define THRESHOLD 40    // Touch sensitivity threshold

// Variables stored in RTC memory (survives deep sleep)
RTC_DATA_ATTR int bootCount = 0;    // Number of wake-ups
RTC_DATA_ATTR int touchCount = 0;   // Number of touch activations

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  
  // Increment boot count
  bootCount++;
  
  // Print wake-up information
  printWakeupReason();
  
  // Configure touch pad for wake up
  setupTouchWakeup();
  
  // Display pattern and current counts
  displayStatus();
  
  // Prepare for sleep
  prepareSleep();
}

void loop() {
  // Never reaches here
}

void setupTouchWakeup() {
  // Configure touch pad as wake up source
  touchSleepWakeUpEnable(TOUCH_PIN, THRESHOLD);
  
  // If woken by touch, increment touch counter
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TOUCHPAD) {
    touchCount++;
  }
}

void printWakeupReason() {
  Serial.println("\n=========================");
  Serial.println("Wake-up Information:");
  Serial.printf("Boot Count: %d\n", bootCount);
  Serial.printf("Touch Count: %d\n", touchCount);
  
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    default:
      Serial.println("First boot or reset");
      break;
  }
  Serial.println("=========================\n");
}

void displayStatus() {
  // Display touch count pattern
  Serial.println("Displaying touch count pattern...");
  
  // Short delay before pattern starts
  delay(500);
  
  // Blink LED according to touch count
  for(int i = 0; i < touchCount; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  
  // Long flash to indicate end of pattern
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}

void prepareSleep() {
  Serial.println("\nPreparing for deep sleep...");
  Serial.println("Touch sensor to wake up!");
  Serial.println("=========================");
  Serial.flush();
  
  // Short delay before sleep
  delay(100);
  
  // Start deep sleep
  esp_deep_sleep_start();
}