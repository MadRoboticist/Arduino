//include libraries
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <avr/io.h>
#include <util/delay.h>
#include <SoftwareSerial.h>
//#include <avr/wdt.h>    

//define pins
#define NeoPIN PB2
#define TXpin PB0
#define RXpin PB1
#define BUZZER PB5
#define UARTdelay 2
uint8_t neopix[16][3];//array for neopixel values
//int16_t uartIN[33][2]; //16 bit 2d array 
boolean BUZZ = false;
boolean UART = false;

//define neopixel ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, NeoPIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial v2vSerial = SoftwareSerial(RXpin, TXpin);
/************************** SETUP FUNCTION *******************/
//function to initialize unit
void setup() {
  //clock_prescale_set(clock_div_1);
  uint8_t i=0;
  uint32_t j=0; 
  
  //run a quick rainbow to check pixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  rainbow(5);   // run the rainbow demo once
   
  //test the buzzer
  digitalWrite(BUZZER, HIGH);   //set buzzer pin high for OFF
  pinMode(BUZZER, OUTPUT);      //set buzzer pin as output
  //while(i<3){ // toggle the buzzer thrice
    digitalWrite(BUZZER,LOW);   //low for on
    _delay_ms(100);              //quick delay
    digitalWrite(BUZZER, HIGH); //high for off
    //_delay_ms(10);              //quick delay
    //i++;                        //next iteration
  //}
  
  i=0; // reset counter
  //set up UART  
  /*not sure if we need these */
  pinMode(RXpin, INPUT); 
  //pinMode(TXpin, OUTPUT); 
  v2vSerial.begin(19200); //115.2kbps
  v2vSerial.write("$");  //send a character
  while(!v2vSerial.available()){
   
    rainbow(5);
    v2vSerial.write("$"); 
    if(v2vSerial.available()>0){
     break; 
    }
   }
   
  
  clearNeopix(); //turn them off 
}

/****************************** MAIN LOOP *******************/
void loop() {
  //uint8_t pixel = v2vSerial.read();  
  int i=0;
  readUART();
  showPix();
  delay(50);
}

/****************************** READ UART *******************/
void readUART() {
  uint8_t i=0, j=0; //8 bit counters
  int16_t uartIN[33][2]; //16 bit 2d array 
  uint8_t incomingByte;  //8 bit UART byte
  while(i<32 && j<32){ //accept no more than 32 characters, for no more than 32 cycles
    //if there are UART communications waiting...
    if ( v2vSerial.available() > 0 ) { 
                  // read the incoming byte:
                  incomingByte = v2vSerial.read();
                  //v2vSerial.write(incomingByte);
                  if (incomingByte == '$'){  //if the byte received is the start byte...
                    //delay(UARTdelay);
                    while(!v2vSerial.available()){
                      //delayMicroseconds(2000);
                    }
                    incomingByte = v2vSerial.read();
                    //v2vSerial.write(incomingByte);
                    uartIN[j][0] = 0x0000 | incomingByte; 
                    while(!v2vSerial.available()){
                      //delayMicroseconds(2000);
                    }
                    incomingByte = v2vSerial.read();
                    //v2vSerial.write(incomingByte);
                    uartIN[j][1] = 0x0000 | incomingByte;
                    j++; //increase the array index  
                    }
                  //otherwise wait the amount of time it would take to read 2 more bytes
                  else { 
                   delayMicroseconds(2000); 
                  }
                  i++; //either way, increase the cycle counter             
    }
    //otherwise wait the amount of time it would take to read 3 bytes
    else { 
       delayMicroseconds(2500);
       i++; //and increase the cycle counter
    }
  }
  //after the 32 cycles are completed...
  uartIN[j][0]=-1; //set the current byte as -1 to signal the end of the array
  writePixArr(uartIN); //convert the input to values in the pixel array
  return; //and then return to the main loop
  }  
  
/************************************* WRITE NEOPIXEL ARRAY **************************/
void writePixArr(int16_t uartIN[33][2]){
   //local variables
   uint8_t dir1, dir2, siz, color, temp; //bytes
   int16_t dir; //unsigned
   uint8_t i; //8 bit counter
   int16_t j; //16 bit counter
   
   //decay the neopixel array to zero
   decayPix();
   
   //loop through the input array
   for(i=0;i<32;i++){ 
     if ( uartIN[i][0] ==-1 ){//if the array has ended, return
      return; 
     }
     
     else { //otherwise shift the value right by 3 because the direction is the MS 5 bits
      UART = true;
      dir = ( uartIN[i][0] >> 3 ) & 0x001F; 
      siz = uartIN[i][0] & 0x0007; //size is LS 3 bits of byte 1
      color = uartIN[i][1] & 0x00FF; //color is byte 2
      //clearNeopix();
      //strip.setPixelColor(dir/2, 25, 0, 0);
      //strip.setPixelColor(siz, 0, 25, 0);
      //strip.setPixelColor(color%16, 0, 0, 25);
     }
     //if direction is 0 or a multiple of 2, it is a single pixel
     if(dir%2 == 0){
       dir1 = dir/2;
       dir2 = dir/2; 
     }
     //otherwise it is between 2 pixels
     else{
       dir2 = (dir + 1)/2;
       if (dir2 == 16){
        dir2 = 0; 
       }
       dir1 = (dir - 1)/2;
     }
     //loop covers bottom half of targeted pixels in size
     for(j=(dir1-siz/2);j<=dir1;j++){  
       if(j<0){//if it's below zero, adjust it to 0-15
        temp = j + 16; 
       }
       else { //otherwise use it as is
        temp = j; 
       }     
       if( color > neopix[temp][0] || neopix[temp][1]==0) {
        neopix[temp][0] = color; 
        neopix[temp][1] = 1;
        neopix[temp][2] = 255;
       }
     } 
     //loop covers upper half of targeted pixels in size
     for(j=dir2;j<=(dir2 + siz/2);j++){  
       if(j>15){ //if it's greater than 15, adjust it to 0-15
        temp = j - 16; 
       }
       else { //otherwise use it as is
        temp = j; 
       } 
       //if the color is a higher priority than what is there...    
       if( color > neopix[temp][0] || neopix[temp][1]==0) {
        //write it to the neopixel array
        neopix[temp][0] = color; 
        neopix[temp][1] = 1;
        neopix[temp][2] = 255;
       }
     }   
   }//end FOR LOOP
   return; 
  }//end function
  
/************************************* SHOW PIXELS **************************/
void showPix(){
   uint8_t color, night, RED, GREEN; //8 bit values
   uint8_t i; //8 bit counter
  //cycle through neopixel array
  
  for(i=0;i<16;i++){
     night = ( neopix[i][0] >>7 ) & B00000001; //check the 'night' bit, which is MSB
     color = neopix[i][0] & B01111111; //color is LS 7 bits
     if(color == 0){
      strip.setPixelColor(i, 0, 0, 0); 
     }
     else{
     RED = color * ( 1 + night )/2; //convert color and night bit to red value
     GREEN = (127-color) * ( 1 + night )/2; //and green value
     strip.setPixelColor(i, RED * neopix[i][2]/255, GREEN * neopix[i][2]/255, 0); //use these values to write the color
     }
     if(color > 0x6B){
      BUZZ = true; 
     }
     
     
  }//end FOR LOOP 
 
  //set off the buzzer if necessary
  if(BUZZ){
   digitalWrite(BUZZER, !digitalRead(BUZZER)); 
   BUZZ = false;
  }
  else digitalWrite(BUZZER, HIGH);
  //v2vSerial.end();
  strip.show(); //show the pixels
  
  //v2vSerial.begin(19200);
  return;
}

/****************************** DECAY PIXELS *********************************/
void decayPix(){
 uint8_t i; //8 bit counter
 //loop through the ring
 for(i=0;i<16;i++){
  if(UART){
  neopix[i][2] = neopix[i][2]*1/2;
  }
  else if (neopix[i][2] == 255){
  neopix[i][2] = neopix[i][2]*7/8;
  }
  else neopix[i][2]= neopix[i][2]*5/8;
  if(neopix[i][2] < 60){
   neopix[i][2] = 0; 
   neopix[i][0] = 0;
  }
  neopix[i][1]=0;
 }
 UART = false;
return; 
}

//function to use buzzer
  
/********************************* STARTUP DEMO FUNCTIONS ***************************/
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
//function to clear the pixel array
void clearNeopix(){
 uint8_t i;
 for(i=0;i<16;i++){
  strip.setPixelColor(i, 0, 0, 0);
  neopix[i][0] = 0;
  neopix[i][1] = 0;
  neopix[i][2] = 0;
  neopix[i][0] = 0;
  neopix[i][1] = 0;
  neopix[i][2] = 0; 
}
strip.show();
return;
}

void delay_us(int us){
  delayMicroseconds(us);
  return; 
  
}


