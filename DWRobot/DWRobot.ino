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
  dw = new DW(&Pw);
  digitalWrite(ledPin, HIGH);
  //Serial.begin(9600);
   
}

//main loop
void loop() 
{ 
    dw->update();
}



