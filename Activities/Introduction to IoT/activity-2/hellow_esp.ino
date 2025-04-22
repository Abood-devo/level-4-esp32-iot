// Define which pin the LED is connected to
// On most ESP32 boards, there's a built-in LED on pin 2
const int ledPin = 2;

// This function runs once when the board starts
void setup() {
  // Start serial communication at 115200 baud rate
  // This lets us send messages to the computer
  Serial.begin(115200);
  
  // Wait a moment for the serial connection to start
  delay(1000);
  
  // Send our first message
  Serial.println("Hello! ESP32 is starting up!");
  
  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);
  
  // Send another message
  Serial.println("LED pin is ready!");
}

// This function runs over and over again
void loop() {
  // Turn LED on
  digitalWrite(ledPin, HIGH);
  // Send message when LED turns on
  Serial.println("LED is ON!");
  // Wait for 1 second (1000 milliseconds)
  delay(1000);
  
  // Turn LED off
  digitalWrite(ledPin, LOW);
  // Send message when LED turns off
  Serial.println("LED is OFF!");
  // Wait for 1 second
  delay(1000);
}



// Try These Modifications:

// 1 Change the delay times to make the LED blink faster or slower:
// delay(500);  // Makes it blink twice as fast

// 2. Add your own messages:
// Serial.println("Your custom message here!");

// 3. Make the LED blink in a pattern:
// digitalWrite(ledPin, HIGH); 
// delay(200);
// digitalWrite(ledPin, LOW);
// delay(200);
// Longer pause
// delay(1000);