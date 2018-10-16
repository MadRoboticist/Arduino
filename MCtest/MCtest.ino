// Sweep
// by BARRAGAN <http://barraganstudio.com>

// http://arduiniana.org/libraries/pwmservo/

//   Board                     SERVO_PIN_A   SERVO_PIN_B   SERVO_PIN_C
//   -----                     -----------   -----------   -----------
//   Arduino Uno, Duemilanove       9            10          (none)
//   Arduino Mega                  11            12            13
//   Sanguino                      13            12          (none)
//   Teensy 1.0                    17            18            15
//   Teensy 2.0                    14            15             4
//   Teensy++ 1.0 or 2.0           25            26            27
//   Teensy LC & 3.x                 (all PWM pins are usable)

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int spd = 1350;    // variable to store the servo position
char temp;
void setup() {
  myservo.attach(3,1100,1800);  // attaches the servo on pin 3 to the servo object
  Serial.begin(115200);
  myservo.writeMicroseconds(spd);
}


void loop() {
  while(!Serial.available()){};
  temp = Serial.read();
  if(temp=='+') spd+=20;
  else if(temp=='-') spd-=20;
  myservo.writeMicroseconds(spd);
  Serial.println(spd);
}
