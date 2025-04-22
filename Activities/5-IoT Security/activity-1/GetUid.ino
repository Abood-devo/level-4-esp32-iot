#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    5
#define RST_PIN   21

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}