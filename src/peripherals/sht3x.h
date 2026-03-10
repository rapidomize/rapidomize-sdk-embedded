#ifndef RPZ_SHT3X_H_
#define RPZ_SHT3X_H_

#include "i2c.h" 

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
/* 
    SHT3x Temperature and Humidity Sensor
 */

namespace rpz{

const char *SHT3X_MSG  = R"("temperature":%.2f, "humidity":%.2f)"; 

class Sht3x: public I2C{

    Adafruit_SHT31 sht3x = Adafruit_SHT31();

    public: 
    Sht3x(Preferences *prefs, ConProvider *conprv, int seq=1):I2C(prefs, conprv, seq){
        sprintf(name, "SHT3x_%d", seq);

        /* defaults can be overridden here
        conf["SDA"] = 4;
        conf["SCL"] = 16; 
        */
        char buf[10];
        sprintf(buf, "%X", SHT31_DEFAULT_ADDR); //default 0x44 - Set to 0x45 for alternate i2c addr
        conf["I2C_ADDRESS"] = buf; 
        configure();
    }

    bool init(JsonDocument *jconf) {
        if(!I2C::init(jconf)) return false;

        if (!sht3x.begin(i2caddr)) {   
            conprv->log(PSTR("%s cannot initiate a connection SDA: %d,  SCL: %d, Address %X\n"), name, sda, scl, i2caddr);
            return false;
        }
        
        inited = true;
        conprv->log(PSTR("%s initialized. SDA: %d,  SCL: %d, Address %X\n"), name, sda, scl, i2caddr);

        return true;
    }

    char * read(){
        if(!inited) return nullptr;

        float temp = sht3x.readTemperature();
        float humidity = sht3x.readHumidity();

        if(!isnan(temp) && !isnan(humidity)){
            sprintf(data, SHT3X_MSG, temp, humidity);
            return data;    
        }   
        conprv->log("%s failed to read temperature & humidity\n",  name);
        return nullptr;
    }

};

}

#endif