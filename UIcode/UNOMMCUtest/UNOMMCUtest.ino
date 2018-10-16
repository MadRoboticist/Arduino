#include <SoftwareSerial.h>
SoftwareSerial testSerial = SoftwareSerial(10, 11);
#define UARTdelay 1
void setup() { 
 //Initialize serial and wait for port to open:
  
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
  testSerial.begin(9600); 
  //while (!testSerial.available()) {
   // ;
  //}
} 

void loop() { 
  uint8_t i, j, k, l;
  uint8_t color=0x6F, dir, siz=0;
  uint8_t byte1, byte2;
  //i=0x72;
  //j=0x5F;
  //k=0x00;
  //l=0x01;
  
  //while(1){
    for(color=0;color<0x7F;color+=4){       
       //for(siz=0;siz<8;siz++){
          for(dir=0;dir<32;dir+=3){
         byte1 = dir * 8;
         byte2 = color;   
         
         //for(i=0;i<5;i++){
         testSerial.write("$");
      delayMicroseconds(10);
      testSerial.write(byte1);
      delayMicroseconds(10);
      testSerial.write(byte2); 
      delay(100);
         }   
    /*
      testSerial.write("$");
      delayMicroseconds(10);
      testSerial.write(i);
      delayMicroseconds(10);
      testSerial.write(j); 
      delay(UARTdelay);
      testSerial.write("$");
      delayMicroseconds(10);
      testSerial.write(k);
      delayMicroseconds(10);
      testSerial.write(l); 
      //delay(UARTdelay);
      //testSerial.write("$");
      //delayMicroseconds(833);
      //testSerial.write(i+8);
      //delayMicroseconds(833);
      //testSerial.write(j+8); 
      //delay(UARTdelay);
      //testSerial.write("$");
      //delayMicroseconds(833);
      //testSerial.write(i+12);
      //delayMicroseconds(833);
      //testSerial.write(j+12); 
      //delay(UARTdelay);
      //delay(1000);   
    delayMicroseconds(6000);
    */
    }
   //}   
  //} 
 //}
}

