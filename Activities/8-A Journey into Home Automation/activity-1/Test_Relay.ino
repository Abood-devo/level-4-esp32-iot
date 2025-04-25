const int relay = 26;

void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
}

void loop() {
  // Turn ON LED via relay (simulate lamp ON)
  digitalWrite(relay, LOW); // LOW activates relay in NO config
  Serial.println("LED ON");
  delay(2000);
  
  // Turn OFF LED via relay (simulate lamp OFF)
  digitalWrite(relay, HIGH);
  Serial.println("LED OFF");
  delay(2000);
}