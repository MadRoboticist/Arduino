
#define BAUD 230400
#define PORT Serial1

void setup() {
  Serial.begin(BAUD);
  PORT.begin(BAUD);
}

void loop() {
  if(Serial.available()) PORT.write(Serial.read());
  if(PORT.available()) Serial.write(PORT.read());
}
