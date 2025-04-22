#include <SPI.h>
#include <MFRC522.h>

// Pin definitions
#define SS_PIN    5    // SDA
#define RST_PIN   21   // RST
#define GREEN_LED 4
#define RED_LED   2
#define BUZZER    15

MFRC522 rfid(SS_PIN, RST_PIN);

// Authorized card UIDs (replace with your card UIDs)
byte authorizedCard[4] = {0x00, 0x00, 0x00, 0x00};

void setup() {
  Serial.begin(115200);
  
  // Initialize SPI bus
  SPI.begin(18, 19, 23, 5); // SCK, MISO, MOSI, SS
  rfid.PCD_Init();
  
  // Initialize outputs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  Serial.println("Scan your card...");
}

void loop() {
  // Reset LEDs
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  
  // Check for new cards
  if (!rfid.PICC_IsNewCardPresent())
    return;
    
  // Read the card
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Check if authorized
  if (checkCard()) {
    accessGranted();
  } else {
    accessDenied();
  }
  
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

bool checkCard() {
  return rfid.uid.uidByte[0] == authorizedCard[0] &&
         rfid.uid.uidByte[1] == authorizedCard[1] &&
         rfid.uid.uidByte[2] == authorizedCard[2] &&
         rfid.uid.uidByte[3] == authorizedCard[3];
}

void accessGranted() {
  Serial.println("Access Granted!");
  digitalWrite(GREEN_LED, HIGH);
  beepSuccess();
  delay(2000);
}

void accessDenied() {
  Serial.println("Access Denied!");
  digitalWrite(RED_LED, HIGH);
  beepError();
  delay(2000);
}

void beepSuccess() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
}

void beepError() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
  }
}