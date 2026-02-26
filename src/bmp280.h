#ifndef RPZ_BMP280_H_
#define RPZ_BMP280_H_

#include "peripheral.h" 

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

/* 
    BMP280 Pressure & Altitude Sensor
 */

namespace rpz{


const char *BMP280_SEN_MSG  = R"({"pressure":%f, "altitude":%f})";    
 
const char *BMP280_tmpl = R"(
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
class BMP280: public Peripheral{

    Adafruit_BMP280 bmp;   

    public: 
        BMP280(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
            sprintf(name, "BMP280_%d", seq);

            String pname = name; pname+="_I2CADDR";
            conf[pname] = BMP280_ADDRESS;

            configure();
            i2caddr = (uint8_t)conf[pname];
        }
        char * confpg(){
            char *fr = (char *) malloc(4096);
            sprintf(fr, BMP280_tmpl, name, name, enabled?"checked":"", i2caddr);
            return fr;
        }
        void init(JsonDocument *jconf) {
            Peripheral::init(jconf);
            if(!enabled) return;

            //Wire.begin(4, 16); //SDA=GPIO4  SCL=GPIO16 

            // Initialize BMP280 sensor
            if (!bmp.begin()) {
                Serial.printf(PSTR("%s cannot initiate a connection address %X\n"), name, i2caddr);
            }else
                Serial.printf(PSTR("%s initialized.\n"), name);
		}

        char * read(){
            if(!enabled) return nullptr;

            // Read data from BMP280
            float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
            float altitude = bmp.readAltitude(1013.25); // Sea level pressure in hPa

            if(!isnan(pressure) && !isnan(altitude)){
                sprintf(data, BMP280_SEN_MSG, pressure, altitude);
                return data;
            }else   
                Serial.printf("%s Failed to read pressure & altitude\n",  name);
            return nullptr;
		}
};

}

#endif