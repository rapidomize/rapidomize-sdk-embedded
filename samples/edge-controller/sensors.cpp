#include "sensors.h" 

#include <Wire.h>
#include "Adafruit_SHT31.h"
/* 
    SHT3x Temperature Humidity
 */

namespace rpz{


const PROGMEM char *SEN_MSG  = R"({"temperature":%f, "humidity":%f})";    

Adafruit_SHT31 sht3x = Adafruit_SHT31();


void Sensors::init(){
    Wire.begin(4, 16); //SDA=GPIO4  SCL=GPIO16 

    if (!sht3x.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.printf("Failed to initialize SHT3x sensor! addr: 0x44");
    }else
        Serial.println("SHT3x sensor initialized.");
}

char * Sensors::read(){
    float temp = sht3x.readTemperature();
    float humidity = sht3x.readHumidity();

    if(!isnan(temp) && !isnan(humidity)){
        sprintf(data, SEN_MSG, temp, humidity);
        return data;    
    }else   
        Serial.println("Failed to read temperature & humidity");
    return nullptr;
}

}

