///////////////////////includes///////////////////////////
#include <Dogwood.h>

////////////////////////functions////////////////////////
//setup function
PWM Pw = PWM();
DW *dw;
const int ledPin = 13;

void setup() 
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
  dw = new DW(&Pw, &Serial);
  digitalWrite(ledPin, HIGH);
  dw->dogwood.turn(true, 20);
}

//main loop
void loop() 
{ 
    dw->update();
}



