
#include <avr/io.h>
#include <util/delay.h>
void moveServo1(int pulse);
void moveServo2(int pulse);
void openLid(int pulse);
void moveServos(int pulse);
void defaultBehavior();
void angry1();
void angry2();
void angry3();
void angry4();

#define BLUE PB2
#define GREEN PB4
#define RED PB0
#define SW1 PB3

int pulse = 0;
boolean UP = true;

void setup(){
 PORTB = 0b00010111;
 DDRB = 0b00010111;
 randomSeed(analogRead(PB5));
 //robot is disturbed
}

void loop(){
 /*for(byte i=0; i<5; i++)
  {
  defaultBehavior();
  }
  for(byte j=0; j<5; j++)
  {
    angry1();  
  }
  /*while(PINB & 0b00001000){}
  
  //while(PINB & 0b00001000){}
   
  while(1){
    for(byte k=0; k<25; k++)
      {
      long rand = random(5);
      switch(rand){
        case 0:
          defaultBehavior();
          break;
        case 1:
          for(byte j=0; j<4; j++)
          {
            angry1();  
          }
          break;
        case 2:
          for(byte k=0; k<2; k++)
          {
            angry2();
          }
          break;
        case 3:
          angry3();
          break;
        case 4:
          angry4();
          break;
        case 5:
          angry5();
          break;
        default:
          defaultBehavior();
          break;
      }
    }
    angry6();
  }*/
 moveServo1(500); 
}

void defaultBehavior()
{
  while(PINB & 0b00001000 && !UP){moveServo1(2200);}
  PORTB = 0b00010110;
  openLid(550);
  UP = true;
  
  while(!(PINB & 0b00001000))
  {
     moveServo1(600);
  }
  pulse = 0;
  PORTB = 0b00010011;
  while(PINB & 0b00001000 && UP)
  { 
    if(pulse <= 200)
    {
      moveServo1(800);    
    }
    else if(pulse <= 800)
    {
      moveServo2(2200);
    }
    else if(pulse <= 1000)
    {
      _delay_ms(1);
    }
    else if(pulse <=2000)
    {
      moveServo1(2200);
    }
    else UP = false;
   pulse += 100; 
  }
  return;
}

void angry1()
{
  while(PINB & 0b00001000 && !UP){moveServo1(2200);}
  PORTB = 0b00010110;
  openLid(550);
  UP = true;
  while(!(PINB & 0b00001000))
  {
     moveServo1(600);
  }
  pulse = 0;
  
  while(PINB & 0b00001000 && UP)
  { 
    if(pulse <= 200)
    {
      moveServo1(700);
    }
    else if(pulse <= 1800)
    {
      _delay_ms(5); 
    }
    else if(pulse <= 2000)
    {
      defaultBehavior(); 
    }
   pulse += 100; 
  }
  return;
}

void angry2()
{
  while(PINB & 0b00001000 && !UP){moveServo1(2200);}
  PORTB = 0b00000111;
  pulse = 0;
  while(!(PINB & 0b00001000))
  {
    if(pulse <= 200)
    {
      openLid(700);
      UP = true;
    }
    else if(pulse <= 1000)
    {
      _delay_ms(5);
    }
    else
    {
      moveServo1(600);
    }
    pulse += 100;
  }  
  pulse = 0;
  while(PINB & 0b00001000 && UP)
  { 
   if(pulse <=200)
   {
     moveServo1(750); 
   }
   else if(pulse <= 1000)
   {
      _delay_ms(1); 
   }
   else if(pulse <= 1200)
   {
     moveServo1(650); 
   }
   else
   { 
     angry1();
     defaultBehavior();
   }
   pulse += 100; 
  }
  return;
}
void angry3()
{
  while(PINB & 0b00001000 && !UP){}
  for(byte i=0; i<6; i++)
  {
   moveServo1(2200); 
  }
  PORTB = 0b00010010;
  openLid(700);
  UP = true;
  _delay_ms(2000);
  for(byte i=0; i<6; i++){
  moveServo2(2200);
  }
  return;
}

void angry4()
{
  while(PINB & 0b00001000 && !UP){moveServo1(2200);}
  for(byte i=0; i<6; i++)
  {
   moveServo1(2200); 
  }
  PORTB = 0b00000110;
  openLid(700);
  UP = true;
  _delay_ms(500);
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  PORTB = 0b00000111;
  openLid(600);
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  PORTB = 0b00000011;
  openLid(600);
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  return;
}

void angry5()
{
  while(PINB & 0b00001000 && !UP){moveServo1(2200);}
  for(byte i=0; i<6; i++)
  {
   moveServo1(2200); 
  }
  PORTB = 0b00010110;
  openLid(800);
  UP = true;
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  PORTB = 0b00010011;
  openLid(800);
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  PORTB = 0b00010010;
  openLid(800);
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  PORTB = 0b00000111;
  openLid(800);
  for(byte i=0; i<5; i++){
  moveServo2(2200);
  }
  return;
}

void angry6()
{
  while(PINB & 0b00001000 && !UP){moveServo1(2200);}
  for(byte i=0; i<6; i+=2)
  {
   moveServo1(2200); 
  }
  PORTB = 0b00010110;
  openLid(600);
  for(byte j=0; j<100; j+=2){
  delay(100-j);
  PORTB ^= 0x10;
  }
  PORTB = 0b00000111;
  for(byte k=0; k<100; k+=2){
  delay(100-k);
  PORTB ^= 0x04;
  }
  PORTB = 0b00010011;
  for(byte l=100; l>0; l-=2){
  delay(100-l);
  PORTB ^= 0x10;
  }
  PORTB = 0b00010011;
  for(byte m=100; m>0; m--){
  delay(100-m);
  PORTB ^= 0x04;
  }
  PORTB = 0b000010111;
  delay(2000);
  PORTB = 0b00010110;
  delay(3000);
  UP = true;
  for(byte i=0; i<6; i++){
  moveServo2(2200);
  }
  return;
}


void moveServo1(int pulse){
  byte hold = PORTB;
  for(byte i=0; i<4; i++){
  PORTB &= 0b11111110;
  _delay_us(20000-pulse);
  PORTB ^= 0b00000001;
  _delay_us(pulse);
  }
  PORTB = hold; 
}

void moveServo2(int pulse)
{
  byte hold = PORTB;
  for(byte i=0; i<5; i++){
  PORTB &= 0b11111101;
  _delay_us(20000-pulse);
  PORTB ^= 0b00000010;
  _delay_us(pulse);
  } 
  PORTB = hold;

}

void openLid(int pulse)
{
  byte hold = PORTB;
  for(byte i=0; i<10; i++){
  PORTB &= 0b11111101;
  _delay_us(20000-pulse);
  PORTB ^= 0b00000010;
  _delay_us(pulse);
  } 
  PORTB = hold;

}
void moveServos(int pulse){
  byte hold = PORTB;
  for(byte i=0; i<5; i++){
  PORTB &= 0b11111100;
  _delay_us(20000-pulse);
  PORTB ^= 0b00000011;
  _delay_us(pulse);
  } 
  PORTB = hold;

}
