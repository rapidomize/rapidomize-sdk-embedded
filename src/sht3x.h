#ifndef RPZ_SHT3X_H_
#define RPZ_SHT3X_H_

#include "peripheral.h" 

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
/* 
    SHT3x Temperature Humidity
 */

namespace rpz{


const char *SEN_MSG  = R"({"temperature":%f, "humidity":%f})";   

const char *Sht3x_tmpl = R"(
<div  class="card">
    <div class="row pos-r"><h3>%s</h3><input type="checkbox" name="%s" %s  class="pos-a" style="right: 0px;"></div>
    <table>
        <tr><td>Interface</td><td>I2C</td></tr>
        <tr><td>SDA GPIO</td><td><input type="number" name="SDA" value="4" disabled></td></tr>
        <tr><td>SCL GPIO</td><td><input type="number" name="SCL" value="16" disabled></td></tr>
        <tr><td>I2C Address (hex)</td><td><input type="text" name="I2C_ADDRESS" value="%X" disabled></td></tr>
    </table>  
</div>  
)";   

class Sht3x: public Peripheral{

    Adafruit_SHT31 sht3x = Adafruit_SHT31();

    public: 
    Sht3x(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
        sprintf(name, "Sht3x_%d", seq);

        String pname = name; pname+="_I2CADDR";

        //default
        conf[pname] = 0x44; // Set to 0x45 for alternate i2c addr
        configure();

        i2caddr = (uint8_t)conf[pname];
    }
    char * confpg(){
        char *fr = (char *) malloc(4096);
        sprintf(fr, Sht3x_tmpl, name, name, enabled?"checked":"", i2caddr);
        return fr;
    }

    void init(JsonDocument *jconf) {
        Peripheral::init(jconf);
        if(!enabled) return;
        
        Wire.begin(4, 16); //SDA=GPIO4  SCL=GPIO16 

        if (!sht3x.begin(i2caddr)) {   
            Serial.printf(PSTR("%s cannot initiate a connection address %X\n"), name, i2caddr);
        }else
            Serial.printf(PSTR("%s initialized.\n"), name);
    }

    char * read(){
        if(!enabled) return nullptr;

        float temp = sht3x.readTemperature();
        float humidity = sht3x.readHumidity();

        if(!isnan(temp) && !isnan(humidity)){
            sprintf(data, SEN_MSG, temp, humidity);
            return data;    
        }else   
            Serial.println("Failed to read temperature & humidity");
        return nullptr;
    }

};

}

#endif