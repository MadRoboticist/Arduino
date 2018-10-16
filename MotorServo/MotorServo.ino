//  Demo function:The application method to drive the DC motor.
//  Author:Frankie.Chu
//  Date:20 November, 2012

#include "Servo.h"
/********** define motor and servo pins ************/
#define Servo_Pin 9
#define Motor_Pin1 12
#define Motor_Pin2 13
#define Motor_Speed 10
#define Servo_Angle 8
//#define Servo_Power 11
#define SPEED 255
#define LEFT 50
#define RIGHT 0
#define STRAIGHT 25
Servo Steering;


void setup()
{
  Steering.write(STRAIGHT);
  //digitalWrite(Servo_Pin, HIGH);
  Steering.attach(Servo_Angle);
  pinMode(Motor_Pin1, OUTPUT);
  pinMode(Motor_Pin2, OUTPUT);
  pinMode(Servo_Pin, OUTPUT);
  pinMode(Motor_Speed, OUTPUT);
  pinMode(Servo_Angle, OUTPUT);
  //pinMode(Servo_Power, OUTPUT);
  //digitalWrite(Servo_Power, HIGH);
  digitalWrite(Servo_Pin, HIGH);
    
}
 
void loop()
{
	Forward(SPEED);
	delay(2000);
        Left(LEFT);
        delay(1000);
        Straight();
        delay(1000);
        Right(RIGHT);
        delay(1000);
        Straight();
	Stop();
	delay(1000);
	Backward(SPEED);
	delay(2000);
	Stop();
	delay(1000);
	
	
}

void Forward(int Speed){
   digitalWrite(Motor_Pin1,HIGH);
   digitalWrite(Motor_Pin2,LOW);
   analogWrite(Motor_Speed, Speed);
}

void Stop(){
   digitalWrite(Motor_Pin1,LOW);
   digitalWrite(Motor_Pin2,LOW);  
}


void Backward(int Speed){
   digitalWrite(Motor_Pin1,LOW);
   digitalWrite(Motor_Pin2,HIGH);
   analogWrite(Motor_Speed, Speed);
}

void Right(int RightAngle){
//analogWrite(Servo_Angle, RightAngle);
Steering.write(RightAngle);

}


void Left(int LeftAngle){
   //analogWrite(Servo_Angle, LeftAngle);
  Steering.write(LeftAngle); 
}

void Straight(){
  //analogWrite(Servo_Angle, STRAIGHT); 
  Steering.write(STRAIGHT);
}
