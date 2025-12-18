#include "sensor.h" 

#include <DS18B20.h>

namespace rpz{

const PROGMEM char *SEN_MSG  = R"({"temperature01":%f, "temperature02":%f})";   

DS18B20 ds1(33);  //channel-1-DS18b20
DS18B20 ds2(14);  //channel-2-DS18b20

void Sensor::init(){
}
/*DS18B20 temperature detect*/
char * Sensor::read(){
  float temp1 = ds1.getTempC();
  delay(500); 
  float temp2 = ds1.getTempC();

  if(!isnan(temp1) && !isnan(temp2)){
      sprintf(data, SEN_MSG, temp1, temp2);
      return data;    
  }else   
    Serial.println("Failed to read temperature");
    return nullptr;
} 

void *Sensor::write(char *){
  return nullptr;
}

void *Sensor::exec(char *){
  return nullptr;
} 

}