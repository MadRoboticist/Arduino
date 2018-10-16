#include <KingRichard.h>

KingRichard *KR;

void setup() {
  Serial.begin(115200);
  Serial1.begin(230400);
  delay(2000);
  Serial.println("Serial initialized");
  KR = new KingRichard(&Serial, &Serial1, &Serial2, &Wire);
}

void loop() 
{
  if(KR->mode() == MANUAL)
  {
    KR->manualRC();  
  }
  else if(KR->mode() == AUGMENTED)
  {
    KR->PID();
  }
  else if(KR->mode() == AUTONOMOUS)
    KR->autonomous();
}



