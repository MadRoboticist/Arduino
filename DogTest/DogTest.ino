/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 16 servos, one after the other

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

//#include <Wire.h>
#include <i2c_t3.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
#define SERVOMID (SERVOMIN+SERVOMAX)/2

// our servo # counter
uint8_t servonum = 4;
bool dir[16]={};
uint16_t pulselen=SERVOMIN; 

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  for(int i=0;i<16;i++) pwm.setPWM(i,0,SERVOMID);
}

void loop() {
  uint8_t inChar;// Drive each servo one at a time
  //uint8_t servonum=0;
  if(Serial.available()){
     inChar=Serial.read();
     switch(inChar){
      case '0':
        servonum=0;
        break;
      case '1':
        servonum=1;
        break;
      case '2':
        servonum=2;
        break;
      case '3':
        servonum=3;
        break;
      case '4':
        servonum=4;
        break;
      case '5':
        servonum=5;
        break;
      case '6':
        servonum=6;
        break;
      case '7':
        servonum=7;
        break;
      case '8':
        servonum=8;
        break;
      case '9':
        servonum=9;
        break;
      case 'a':
      case 'A':
        servonum=10;
        break;
      case 'b':
      case 'B':
        servonum=11;
        break;
      case 'c':
      case 'C':
        servonum=12;
        break;
      case 'd':
      case 'D':
        servonum=13;
        break;
      case 'e':
      case 'E':
        servonum=14;
        break;
      case 'f':
      case 'F':
        servonum=15;
        break;
      case '-':
      case '_':
        pulselen-=5;
        break;
      case '=':
      case '+':
        pulselen+=5;
        break;
     }
     /*
     if(dir[servonum]==false)
     {
          for (pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
          {
            pwm.setPWM(servonum, 0, pulselen);
          }
          dir[servonum]=true; 
     }
     else
     {
          for (pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
          {
            pwm.setPWM(servonum, 0, pulselen);
          }
          dir[servonum]=false;
     }
     */
     
        Serial.print(servonum);
        Serial.print(" ");
        Serial.println(pulselen);
    }
    pwm.setPWM(servonum,0,pulselen);
}
