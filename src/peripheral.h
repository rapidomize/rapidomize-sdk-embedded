#ifndef RPZ_SENSOR_H_
#define RPZ_SENSOR_H_

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>


namespace rpz{

struct KV{
    char name[64];
    union{
        int ivalue;
        float fvalue;
        char svalue[100];
    };
};   

class Peripheral{
    public: 
        Peripheral(Preferences *prefs, int seq=1):seq(seq){
            this->prefs = prefs;
        }

        virtual bool configure(){
            String sconf = prefs->getString(name);
            if(sconf.length() != 0){
                auto err = deserializeJson(conf, sconf.c_str());
                if(err){
                    Serial.printf(PSTR("Error parsing configuration for %s : %s\n"), name, sconf.c_str());
                    return false;
                }
            }
            conf[name] = enabled = (bool)conf[name] | false;
            hasenabled = enabled;

            //TODO: debug
            serializeJson(conf, sconf);
            Serial.printf("%s [%d] ==> %s\n", name, enabled?1:0, sconf.c_str());
            
            return true;

        }
        /* virtual bool configure(const char *dconf){
            String sconf = prefs->getString(name, dconf);
            auto err = deserializeJson(conf, sconf.c_str());
            if(err){
                Serial.printf(PSTR("Error parsing configuration for %s : %s\n"), name, sconf.c_str());
                return false;
            }
            conf[name] = enabled = (bool)conf[name] | false;
            serializeJson(conf, sconf);
            Serial.printf(PSTR("enabled %d ==> %s\n"),enabled, sconf.c_str());
            hasenabled = enabled;
            return true;
        } */
        virtual char * confpg() = 0;
        virtual void init(JsonDocument *jconf) {
            if(!jconf) return;

            //for now only enable/disable
            const JsonDocument _jconf = *jconf;
            hasenabled = enabled;
            enabled = conf[name] = _jconf[name] == "on";
            String sconf;
            serializeJson(conf, sconf);
            Serial.printf(PSTR("\ninit1: %s\n"), sconf.c_str());
            prefs->putString(name, sconf.c_str());
        }
        virtual char * read(){
			return nullptr;
		}
        virtual void *write(char *data){
			return nullptr;
		};
        virtual void *exec(char *fn){
			return nullptr;
		}
        
        /* virtual bool hasev(){
            if(triggered){
                Serial.println("triggered");
                unsigned long now = (unsigned long) (esp_timer_get_time() / 1000ULL);
                if (now - lsttime > DEBOUNCE_DELAY ) {
                    lsttime = now;
                    Serial.println("triggered - true");
                    return true;
                }
            }
			return false;
		} */

        virtual ~Peripheral() = default;

        bool isr = false;

    protected:
        Preferences *prefs;
        JsonDocument conf;
        bool enabled = false;
        bool hasenabled = false;
        char data[512];
        int seq;
        char name[60];

        uint8_t i2caddr;
        uint8_t rs485addr; //slave address
};

}

#endif /* RPZ_SENSOR_H_ */
