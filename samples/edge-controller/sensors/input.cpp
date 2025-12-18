/*Input triggers the Buzzer code*/
#include "sensor.h" 

#include <DS18B20.h>

namespace rpz{

const int IN1=36;
const int IN2=39;

void Sensor::init(){
  pinMode(IN1,INPUT);
  pinMode(IN2,INPUT);
  pinMode(12,OUTPUT);
}


char * Sensor::read(){
  if(digitalRead(IN1)==0){
    Serial.print("input01 is on");
    digitalWrite(12,HIGH);
    delay(1000); 
    digitalWrite(12,LOW) ;
    delay(1000);
  }
  
  if(digitalRead(IN2) == 0){
    Serial.print("input02 is on");
    Serial.println("");
    digitalWrite(12,HIGH);
    delay(1000); 
    digitalWrite(12,LOW) ;
    delay(1000);
  } 

  return nullptr;
} 

void *Sensor::write(char *){
  return nullptr;
}

void *Sensor::exec(char *){
  return nullptr;
} 

}


