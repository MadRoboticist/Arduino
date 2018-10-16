//#include <i2c_t3.h>
//#include <RCcontrol.h>
//#include <KRmotors.h>
//#include <UBLOX.h>

//RCcontrol* RC;
//KRmotors* motors;
//UBLOX* gps;

void setup() {
  //Wire.begin(I2C_MASTER, 0x00,I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100);
  Serial.begin(250000);
  delay(500);
  Serial.println("Serial initialized");
  //RC = new RCcontrol(&Serial);
  Serial.println("RC control initialized");
  //motors = new KRmotors(&Serial);
  Serial.println("Motor control initialized");
  //gps = new UBLOX(&Serial, &Wire);
  Serial.println("GPS initialized");
}

void loop() {
  Serial.println("printing..");
  /*if(!RC->getESTOP())
  {
  uint8_t mode = RC->getMode();
  if(mode == MANUAL){
    Serial.println("Manual RC");
    motors->RCmix(RC->getThrottle(),RC->getSteering());
  }
  else if(mode == AUGMENTED){
    Serial.println("Augmented RC");
    //motors->AutonRCMix(RC->getThrottle(),RC->getSteering(),gps->getHeading());
  }
  else motors->AutonMix(0,0);
  }*/
}
