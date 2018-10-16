#define SIO_C A5
#define SIO_D A4
#define SIO_CLOCK_DELAY 100
#define HREF 3
#define PCLK 2
#define XCLK 11
 boolean D[8] = {A0,A1,A2,A3,4,5,6,7};
 //int OF = 0;
void setup()
{  
 
 // digitalWrite(11,HIGH);delayMicroseconds(SIO_CLOCK_DELAY);   
 // digitalWrite(11,LOW);delayMicroseconds(SIO_CLOCK_DELAY);   
 // digitalWrite(11,HIGH);delayMicroseconds(SIO_CLOCK_DELAY);   
 cli();//disable interrupts
	// Setup the 8mhz PWM clock 
	 // This will be on pin 11
	DDRB|=(1<<3);//pin 11
	ASSR &= ~(_BV(EXCLK) | _BV(AS2));
	TCCR2A=(1<<COM2A0)|(1<<WGM21)|(1<<WGM20);
	TCCR2B=(1<<WGM22)|(1<<CS20);
	OCR2A=-0.5;//(F_CPU)/(2*(X+1))
sei();//enable interrupts

for(int j=0;j<8;j++){
  pinMode(D[j],INPUT);
}
pinMode(HREF,INPUT);
pinMode(PCLK,INPUT);
 Serial.begin(9600);
  Serial.println("Start InitOV7670 test program");
  if(InitOV7670())    
    Serial.println("InitOV7670 OK");
  else
    Serial.println("InitOV7670 NG");
    
    Config_OV7670();
  
}
 int x=0;
void loop()
{if(x<50){
while(digitalRead(HREF)==1){}
  Serial.println("HREF falling edge");
  Serial.println(analogRead(HREF),HEX);
  while(digitalRead(HREF)==0){}
  Serial.println("HREF rising edge");
  Serial.println(analogRead(HREF),HEX);
}
  if(x<50){
    for(int i=0;i<8;i++){
 // Serial.print(digitalRead(D[i]),BIN);
    }
  } 
  
  x++;

/* if(OF<100){
  if(!HREF){Serial.println("");}
  while(!HREF){}
  while(!PCLK){}
  for(int i=7;i>=0;i--){
  Serial.print(D[i],BIN);
  Serial.print(" ");
  }
  while(PCLK){
  switch(11){
  case HIGH: Serial.println("HIGH"); break;
  case LOW:  Serial.println("LOW"); break;
  }}
  Serial.println(" ");
  OF++;}*/
  }

int InitOV7670(void)
{
  char temp;
  int i=0;
 
  InitSCCB();
 
  temp=0x80;
  if(WriteOV7670(0x12, temp)==0) //Reset SCCB
  {
    Serial.println("Resetting SCCB NG");
    return 0 ;
  }
  //Serial.println("Resetting SCCB OK");
  //delayMicroseconds(10);
 
//  for(i=0;i<CHANGE_REG_NUM;i++)
//  {
//    if( 0==wrOV7670Reg(pgm_read_byte( &change_reg[i][0]),pgm_read_byte( &change_reg[i][1]) ))
//    {
//      return 0;
//    }
//  }
 
  return 0x01; //ok
} 
void InitSCCB(void) //SCCB初期化
{
  pinMode(SIO_C,OUTPUT);
  pinMode(SIO_D,OUTPUT);
  digitalWrite(SIO_C,HIGH);
  digitalWrite(SIO_D,HIGH);
  Serial.println("InitSCCB - PortDirectionSet & Set High OK");
}
char SCCBWrite(byte m_data)
{
  unsigned char j,tem;
  
  //Serial.print("SCCBWrite 0x");
  //Serial.println(m_data,HEX);
 
  //Serial.print("SCCBWrite");
  for(j=0;j<8;j++) //循环8次发送数据
  {
    if((m_data<<j)&0x80)
    {
      digitalWrite(SIO_D,HIGH);
    }
    else
    {
      digitalWrite(SIO_D,LOW);
    }
    delayMicroseconds(SIO_CLOCK_DELAY);
    digitalWrite(SIO_C,HIGH);
    delayMicroseconds(SIO_CLOCK_DELAY);
    digitalWrite(SIO_C,LOW);
    delayMicroseconds(SIO_CLOCK_DELAY);
  }
 
  //Serial.println("");
  //delayMicroseconds(SIO_CLOCK_DELAY);
  digitalWrite(11,LOW); //debug
  pinMode(SIO_D,INPUT); //SIO_Dのバスをスレーブ(OV7670)に渡す
  digitalWrite(SIO_D,LOW); //プルアップ防止
  delayMicroseconds(SIO_CLOCK_DELAY);
 
  digitalWrite(11,HIGH); //debug
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
 
  //Serial.println(" Write done");
  digitalWrite(11,LOW); //debug
  if(digitalRead(SIO_D)==HIGH)
  {
    //SIO_D=Hなら失敗
    tem=0;
    Serial.println("SCCBWrite NG");
  }
  else
  {
    //SIO_D=Lなら成功
    tem=1; 
    //Serial.println("SCCBWrite OK");
  }
  digitalWrite(SIO_C,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);	
  pinMode(SIO_D,OUTPUT); //SIO_Dのバスをマスター(Arduino)に戻す
  //delayMicroseconds(SIO_CLOCK_DELAY);	
  //digitalWrite(SIO_D,LOW);
  //delayMicroseconds(SIO_CLOCK_DELAY);	
 
  //pinMode(SIO_C,OUTPUT); //SIO_Cのバスをマスター(Arduino)に戻す
 
  return tem;  
} 

 
void StartSCCB(void) //SCCBスタート
{
  //Serial.println("StartSCCB");
 
  digitalWrite(SIO_D,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
  digitalWrite(SIO_D,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);
  digitalWrite(SIO_C,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);
}
 
void StopSCCB(void) //SCCBストップ
{
  //Serial.println("StopSCCB");
 
  digitalWrite(SIO_D,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
  digitalWrite(SIO_D,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
}
 

 
  
 
////////////////////////////
//機能：OV7660レジスタに書き込む
//返値：成功=1 失敗=0
int WriteOV7670(char regID, char regDat)
{
	StartSCCB();
	if(SCCBWrite(0x42)==0)
	{
                Serial.println(" Write Error 0x42");
  		StopSCCB();
		return(0);
	}
	delayMicroseconds(SIO_CLOCK_DELAY);
  	if(SCCBWrite(regID)==0)
	{
		StopSCCB();
		return(0);
	}
	delayMicroseconds(SIO_CLOCK_DELAY);
  	if(SCCBWrite(regDat)==0)
	{
		StopSCCB();
		return(0);
	}
  	StopSCCB();
	//Serial.println("Write Successful");
  	return(1);
}
void Config_OV7670(){
  WriteOV7670(0x12,0x00);
  WriteOV7670(0x40,0xC0);
  WriteOV7670(0x11,0x03);
  WriteOV7670(0x15,0x24);
  WriteOV7670(0x3B,0x10);
  WriteOV7670(0x3E,0x1A);
  WriteOV7670(0x41,0x08);
  WriteOV7670(0x76,0x60);
  WriteOV7670(0x73,0xF2);
  WriteOV7670(0x00,0xFF);
  WriteOV7670(0x4f, 0x80);
    WriteOV7670(0x50, 0x80);
    WriteOV7670(0x51, 0x00);
    WriteOV7670(0x52, 0x22);
    WriteOV7670(0x53, 0x5e);
    WriteOV7670(0x54, 0x80);
    WriteOV7670(0x56, 0x40);
    WriteOV7670(0x58, 0x9e);
    WriteOV7670(0x59, 0x88);
    WriteOV7670(0x5a, 0x88);  /* Oi registers apo 5A mehri 5E iparhoun mono sto implementation guide */
    WriteOV7670(0x5b, 0x44);
    WriteOV7670(0x5c, 0x67);
    WriteOV7670(0x5d, 0x49);
    WriteOV7670(0x5e, 0x0e);
    WriteOV7670(0x69, 0x00);
    WriteOV7670(0x6a, 0x40);
    WriteOV7670(0x6b, 0x0a);
    WriteOV7670(0x6c, 0x0a);
    WriteOV7670(0x6d, 0x55);
    WriteOV7670(0x6e, 0x11);
    WriteOV7670(0x6f, 0x9f);
    WriteOV7670(0xb0, 0x84);
    WriteOV7670(0x17, 0x16);
    WriteOV7670(0x18, 0x04);
    WriteOV7670(0x32, 0x24);
    WriteOV7670(0x19, 0x02);
    WriteOV7670(0x1a, 0x7a);
    WriteOV7670(0x03, 0x0a);
    WriteOV7670(0x0c, 0x04);
    WriteOV7670(0x1e, 0x27);	/* Mirror/VFlip: Enable/Disable (Datasheet - pg.15) */
   WriteOV7670(0x72, 0x22); /* Vertical and Horizontal down sample by 4 */
  
}
