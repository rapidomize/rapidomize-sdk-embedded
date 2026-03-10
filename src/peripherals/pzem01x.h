#ifndef RPZ_PZEM01X_H_
#define RPZ_PZEM01X_H_

#include "modbus.h"

#include <Arduino.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

/* 
    PZEM-01x AC/DC Energy Meter with MAX485 RS485 module
    Uses HardwareSerial (Serial2) for RS485 communication
    Manual DE/RE pin control for MAX485
    Measures: Voltage, Current, Power, Energy, Frequency, Power Factor
 */

namespace rpz{

const char *PZEM_MSG  = R"("voltage":%.2f, "current":%.3f, "power":%.2f, "energy":%.2f)";//, "alarms":%d "frequency":%.1f, "pf":%.2f

class PZEM01x: public Modbus{
        
    public: 

    PZEM01x(Preferences *prefs, ConProvider *conprv, int seq=1):Modbus(prefs, conprv, seq){
        sprintf(name, "PZEM01x_%d", seq);
        
        //defaults
        char buf[10];
        sprintf(buf, "%X", 0x01);
        conf["SLAVE_ADDR"] = buf;  // Default slave address
        
        configure();
    }


    bool init(JsonDocument *jconf) {
       if(!Modbus::init(jconf)) return false;

        inited = true;
        conprv->log(PSTR("%s initialized. Slave: 0x%02X, RX: %d, TX: %d, DE: %d, Baud: %lu\n"), name,
                     slvaddr, rxPin, txPin, dePin, baudRate);
        return true;
    }

    char * read(){
        if(!inited) return nullptr;

        // Read all measurement registers starting from 0x0000
        // PZEM-01x registers: 0x0000-Voltage, 0x0001-Current, 0x0002-Power, 
        // 0x0003-Energy, 0x0004-Frequency, 0x0005-Power Factor
        uint16_t values[6];
        
        if (!readHldRegs(0x0000, 8, values)) {
            Serial.println("Failed to read from PZEM-01x");
            return nullptr;
        }
        
        // Convert raw values to actual values (PZEM-01x scaling factors)
        float voltage = values[0] * 0.01f;     // 0.01V resolution
        float current = values[1] * 0.01f;     // 0.01A resolution
        float power = values[2] * 0.1f;        // 0.1W resolution
        float energy = values[3] * 1.0f;       // 1Wh resolution
        int almH = values[4];                  // High voltage alarm status
        int alm  = values[4] | values[5];      // Low voltage alarm status

        //PZEM-016
        //float frequency = values[4] * 0.1f;    // 0.1Hz resolution
        //float pf = values[5] * 0.01f;          // 0.01 resolution

        sprintf(data, PZEM_MSG, voltage, current, power, energy);//, alm
        return data;
    }

};

}

#endif

