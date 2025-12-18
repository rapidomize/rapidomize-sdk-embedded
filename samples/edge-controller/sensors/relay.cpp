/*Relay output code*/
#include "sensor.h" 

#include <DS18B20.h>

namespace rpz{

const int R1=2;
const int R2=15;

void Sensor::init(){
  pinMode(R1,OUTPUT);
  pinMode(R2,OUTPUT);
}

char * Sensor::read(){
  return nullptr;
}
void *Sensor::write(char *){
  digitalWrite(R1,HIGH);
  delay(1000); 
  digitalWrite(R2,HIGH);
  delay(1000); 
  digitalWrite(R1,LOW) ;
  delay(1000); 
  digitalWrite(R2,LOW) ;
  delay(1000); 
  return nullptr;
}

void *Sensor::exec(char *){
  return nullptr;
} 

}

