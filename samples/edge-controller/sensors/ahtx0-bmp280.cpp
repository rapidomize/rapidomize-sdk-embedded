#include "sensor.h" 

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h> 
#include <Adafruit_BMP280.h>

/* 
    AHT20+BMP280 Temperature Humidity Pressure
 */

namespace rpz{


const PROGMEM char *SEN_MSG  = R"({"temperature":%f, "humidity":%f, "pressure":%f, "altitude":%f})";    

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;    

void Sensor::init(){
    Wire.begin(4, 16); //SDA=GPIO4  SCL=GPIO16 

    // Initialize BMP280 sensor
    if (!bmp.begin()) { 
        Serial.printf(PSTR("Failed to initialize BMP280 sensor! addr: %x"), bmp.sensorID());
    }
    Serial.printf(PSTR("BMP280 sensor initialized. addr: %x\n"), bmp.sensorID());

    if (!aht.begin()) {
        Serial.println("Failed to initialize AHT20 sensor!");
    }
    Serial.println("AHT20 sensor initialized.");
}

char * Sensor::read(){
    // Read data from AHT20
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    // Read data from BMP280
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
    float altitude = bmp.readAltitude(1013.25); // Sea level pressure in hPa

    sprintf(data, SEN_MSG, temp.temperature, humidity.relative_humidity, pressure, altitude);

    return data;
}
void *Sensor::write(char *){
  return nullptr;
}

void *Sensor::exec(char *){
  return nullptr;
} 

}

