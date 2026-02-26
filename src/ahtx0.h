#ifndef RPZ_AHTX0_H_
#define RPZ_AHTX0_H_

#include "peripheral.h" 

#include <Adafruit_AHTX0.h> 

/* 
    AHTX0 Temperature and Humidity Sensor, e.g. AHT10, AHT20
 */

namespace rpz{


const char *AHTx0_SEN_MSG  = R"({"temperature":%f, "humidity":%f})";    

const char *AHTx0_tmpl = R"(
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

class AHTx0: public Peripheral{

    Adafruit_AHTX0 aht;

    public: 
        AHTx0(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
            sprintf(name, "AHTx0_%d", seq);

            String pname = name; pname+="_I2CADDR";

            conf[pname]  = AHTX0_I2CADDR_DEFAULT;
            configure();
            i2caddr = (uint8_t)conf[pname];
        }
        char * confpg(){
            char *fr = (char *) malloc(4096);
            sprintf(fr, AHTx0_tmpl, name, name, enabled?"checked":"", i2caddr);
            return fr;
        }
        void init(JsonDocument *jconf) {
            Peripheral::init(jconf);
            if(!enabled) return;

            //Wire.begin(4, 16); //SDA=GPIO4  SCL=GPIO16 
            // Initialize sensor
            if (!aht.begin()) {
                Serial.printf(PSTR("%s cannot initiate a connection address %X\n"), name, i2caddr);
                return;
            }else Serial.printf(PSTR("%s initialized.\n"), name);
		}

        char * read(){
            if(!enabled) return nullptr;
			// Read data from AHTx0
            sensors_event_t humidity, temp;
            aht.getEvent(&humidity, &temp);

            if(!isnan(temp.temperature) && !isnan(humidity.relative_humidity)){
                sprintf(data, AHTx0_SEN_MSG, temp.temperature, humidity.relative_humidity);
                return data;    
            }else   
                Serial.printf("%s failed to read temperature & humidity\n",  name);
            return nullptr;
		}
};

}

#endif