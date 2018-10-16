#define SCL_PORT PORTC
#define SCL_PIN 5
#define SDA_PORT PORTC
#define SDA_PIN 4

#define XCLK 11
#define PCLK 2
#define HREF 3
int D[8] = {A0,A1,A2,A3,4,5,6,7};
#include <SoftI2CMaster.h>
#include <avr/io.h>
void setup(){
  int i=0;
  for(i=0;i<8;i++){
   pinMode( D[i], INPUT);
  }
  pinMode(XCLK, OUTPUT);
  pinMode(PCLK, INPUT);
  pinMode(HREF, INPUT);
  
  Serial.begin(9600); // change baudrate to 2400 on terminal when low CPU freq!
  Serial.println(F("Intializing ..."));
  Serial.print("I2C delay counter: ");
  Serial.println(I2C_DELAY_COUNTER);
  if (!i2c_init()) 
    Serial.println(F("Initialization error. SDA or SCL are low"));
  else
    Serial.println(F("...done"));
}

void loop(void)
{
  uint8_t add = 0;
  int found = false;
  Serial.println("Scanning ...");

  Serial.println("       8-bit 7-bit addr");
  // try read
  do {
    Serial.println(add, HEX);
    if (i2c_start(add | I2C_READ)) {
      found = true;
      Serial.println(add, HEX);
      i2c_read(true);
     
      i2c_stop();
      Serial.print("Read:   0x");
      if (add < 0x0F) Serial.print(0, HEX);
      Serial.print(add+I2C_READ, HEX);
      Serial.print("  0x");
      if (add>>1 < 0x0F) Serial.print(0, HEX);
      Serial.println(add>>1, HEX);
    } else i2c_stop();
    Serial.println(add, HEX);
    add += 2;
  } while (add);

  // try write
  add = 0;
  do {
    if (i2c_start(add | I2C_WRITE)) {
      found = true;
      Serial.println(add, HEX);
      i2c_stop();
      Serial.print("Write:  0x");    
      if (add < 0x0F) Serial.print(0, HEX);  
      Serial.print(add+I2C_WRITE, HEX);
      Serial.print("  0x");
      if (add>>1 < 0x0F) Serial.print(0, HEX);
      Serial.println(add>>1, HEX);
    } else i2c_stop();
    i2c_stop();
    add += 2;
  } while (add);
  if (!found) Serial.println(F("No I2C device found."));
  Serial.println(add+I2C_WRITE, HEX);
  Serial.println("Done\n\n");
  delay(1000);
}
