void setup(){
  pinMode(11,OUTPUT);
  digitalWrite(11,HIGH);
  pinMode(A5, OUTPUT);
  digitalWrite(A5,HIGH);
  pinMode(A4,OUTPUT);
  digitalWrite(A4,HIGH); 
}
void loop(){
 delay(2000);
digitalWrite(11,LOW);
delay(2000);
digitalWrite(11,HIGH);
  
}
