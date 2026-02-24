#ifndef RPZ_PZEM01X_H_
#define RPZ_PZEM01X_H_

#include "peripheral.h"

#include <Arduino.h>
#include <HardwareSerial.h>

/* 
    PZEM-01x AC/DC Energy Meter with MAX485 RS485 module
    Uses HardwareSerial (Serial2) for RS485 communication
    Manual DE/RE pin control for MAX485
    Measures: Voltage, Current, Power, Energy, Frequency, Power Factor
 */

namespace rpz{

const char *PZEM_tmpl = R"(
<div  class="card">
    <div class="row pos-r"><h3>%s</h3><input type="checkbox" name="%s" %s  class="pos-a" style="right: 0px;"></div>
    <table>
        <tr><td>Interface</td><td>RS485 - Modbus RTU</td></tr>
        <tr><td>RX GPIO</td><td><input type="number" name="RX_PIN" value="35" disabled></td></tr>
        <tr><td>TX GPIO</td><td><input type="number" name="TX_PIN" value="32" disabled></td></tr>
        <!--<tr><td>DE GPIO</td><td><input type="number" name="TX_PIN" value="4" disabled></td></tr>-->
        <tr><td>Baud Rate</td><td><input type="number" name="BAUD_RATE" value="9600" disabled></td></tr>
        <tr><td>Slave Address (hex)</td><td><input type="text" name="SLAVE_ADDR" value="1" disabled></td></tr>
    </table>  
</div>  
)";

const char *PZEM_MSG  = R"({"voltage":%.1f, "current":%.3f, "power":%.1f, "energy":%.3f, "frequency":%.1f, "pf":%.2f})";

// Modbus RTU constants
const uint8_t MODBUS_READ_HOLDING_REG = 0x03;
const uint8_t MODBUS_ERROR_FLAG = 0x80;


class PZEM01x: public Peripheral{
        
    public: 
    const char * dconf = "{\"SLAVE_ADDR\":1, \"DE_PIN\":4,\"RX_PIN\":35, \"TX_PIN\":32, \"BAUD_RATE\":9600}";

    PZEM01x(Preferences *prefs, int seq=1):Peripheral(prefs, seq){
        name = "PZEM01x_";
        name+=seq;
        configure(dconf);

        slaveAddr = conf["SLAVE_ADDR"];  // Default slave address
        baudRate = conf["BAUD_RATE"];    // Default baud rate
        //conf["DE_PIN"] = dePin = 4;    // DE/RE pin for MAX485
        rxPin = conf["RX_PIN"];          // A - RX pin (GPIO35) 
        txPin = conf["TX_PIN"];          // B - TX pin (GPIO32)
    }

    char * confpg(){
        char *fr = (char *) malloc(4096*2);
        const char * pname = name.c_str();
        sprintf(fr, PZEM_tmpl, pname, pname, enabled?"checked":"");//, pname, (int)conf["SLAVE_ADDR"]
        return fr;
    }

    void init(JsonDocument *jconf) {
        Peripheral::init(jconf);
        if(!enabled) return;

        // Parse configuration if provided
        // Format: "slave_addr,de_pin,rx_pin,tx_pin,baud_rate" or use defaults
         Serial.printf(PSTR("%s initialized. Slave: 0x%02X, RX: %d, TX: %d, Baud: %lu\n"), name.c_str(),
                     slaveAddr, rxPin, txPin, baudRate);//DE: %d, dePin
        
        // Configure DE/RE pin
        /* pinMode(dePin, OUTPUT);
        digitalWrite(dePin, LOW); // Start in receive mode */
        
        // Initialize Serial2
        serialPort = &Serial2;
        serialPort->begin(baudRate, SERIAL_8N1, rxPin, txPin);
        
        // Test communication by reading voltage
        delay(1000); // Allow time for initialization
        if (testCommunication()) {
            Serial.println("PZEM-01x communication OK.");
        } else {
            Serial.println("PZEM-01x communication failed. Check wiring and power.");
        }
    }

    char * read(){
        if(!enabled) return nullptr;

        // Read all measurement registers starting from 0x0000
        // PZEM-01x registers: 0x0000-Voltage, 0x0001-Current, 0x0002-Power, 
        // 0x0003-Energy, 0x0004-Frequency, 0x0005-Power Factor
        uint16_t values[6];
        
        if (!readHoldingRegisters(0x0000, 6, values)) {
            Serial.println("Failed to read from PZEM-01x");
            return nullptr;
        }
        
        // Convert raw values to actual values (PZEM-01x scaling factors)
        float voltage = values[0] * 0.1f;      // 0.1V resolution
        float current = values[1] * 0.001f;    // 0.001A resolution
        float power = values[2] * 0.1f;        // 0.1W resolution
        float energy = values[3] * 1.0f;       // 1Wh resolution
        float frequency = values[4] * 0.1f;    // 0.1Hz resolution
        float pf = values[5] * 0.01f;          // 0.01 resolution
        
        sprintf(data, PZEM_MSG, voltage, current, power, energy, frequency, pf);
        return data;
    }

    private:
    uint8_t slaveAddr;
    int dePin;
    int rxPin;
    int txPin;
    uint32_t baudRate;
    HardwareSerial* serialPort;
    
    // Modbus RTU CRC16 calculation
    uint16_t crc16(uint8_t *buffer, uint8_t length) {
        uint16_t crc = 0xFFFF;
        for (uint8_t pos = 0; pos < length; pos++) {
            crc ^= (uint16_t)buffer[pos];
            for (uint8_t i = 8; i != 0; i--) {
                if ((crc & 0x0001) != 0) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }
        return crc;
    }
    
    // Send Modbus RTU request
    bool sendModbusRequest(uint8_t function, uint16_t startAddr, uint16_t regCount) {
        uint8_t request[8];
        
        // Build request
        request[0] = slaveAddr;
        request[1] = function;
        request[2] = (startAddr >> 8) & 0xFF;
        request[3] = startAddr & 0xFF;
        request[4] = (regCount >> 8) & 0xFF;
        request[5] = regCount & 0xFF;
        
        // Calculate CRC
        uint16_t crc = crc16(request, 6);
        request[6] = crc & 0xFF;
        request[7] = (crc >> 8) & 0xFF;
        
        // Set DE/RE pin HIGH for transmit
        //digitalWrite(dePin, HIGH);
        //delayMicroseconds(100);
        
        // Send request
        serialPort->write(request, 8);
        serialPort->flush();
        
        // Set DE/RE pin LOW for receive
        delayMicroseconds(100);
        //digitalWrite(dePin, LOW);
        
        return true;
    }
    
    // Read Modbus RTU response
    int readModbusResponse(uint8_t *buffer, uint8_t expectedBytes, uint32_t timeout = 1000) {
        uint32_t startTime = millis();
        uint8_t index = 0;
        
        while ((millis() - startTime) < timeout) {
            while (serialPort->available() > 0) {
                buffer[index++] = serialPort->read();
                if (index >= expectedBytes) {
                    return index;
                }
            }
        }
        return index; // Return number of bytes read
    }
    
    // Read holding registers]
    bool readHoldingRegisters(uint16_t startAddr, uint16_t regCount, uint16_t *values) {
        // Send request
        if (!sendModbusRequest(MODBUS_READ_HOLDING_REG, startAddr, regCount)) {
            return false;
        }
        
        // Expected response: slave(1) + function(1) + byteCount(1) + data(regCount*2) + CRC(2)
        uint8_t expectedBytes = 5 + regCount * 2;
        uint8_t response[64];
        
        int bytesRead = readModbusResponse(response, expectedBytes);
        if (bytesRead < 5) {
             Serial.printf(PSTR("Modbus response timeout or incomplete. Bytes read: %d\n"), bytesRead);
            return false;
        }
        
        // Check slave address
        if (response[0] != slaveAddr) {
             Serial.printf(PSTR("Wrong slave address in response. Expected: 0x%02X, Got: 0x%02X\n"), 
                            slaveAddr, response[0]);
            return false;
        }
        
        // Check function code (error if MSB set)
        if (response[1] & MODBUS_ERROR_FLAG) {
             Serial.printf(PSTR("Modbus error code: 0x%02X\n"), response[1]);
            return false;
        }
        
        if (response[1] != MODBUS_READ_HOLDING_REG) {
             Serial.printf(PSTR("Wrong function code in response. Expected: 0x%02X, Got: 0x%02X\n"), 
                            MODBUS_READ_HOLDING_REG, response[1]);
            return false;
        }
        
        // Check byte count
        uint8_t byteCount = response[2];
        if (byteCount != regCount * 2) {
             Serial.printf(PSTR("Wrong byte count in response. Expected: %d, Got: %d\n"), 
                            regCount * 2, byteCount);
            return false;
        }
        
        // Verify CRC
        uint16_t receivedCRC = (response[bytesRead - 1] << 8) | response[bytesRead - 2];
        uint16_t calculatedCRC = crc16(response, bytesRead - 2);
        if (receivedCRC != calculatedCRC) {
             Serial.printf(PSTR("CRC error. Received: 0x%04X, Calculated: 0x%04X\n"), 
                            receivedCRC, calculatedCRC);
            return false;
        }
        
        // Extract register values
        for (uint8_t i = 0; i < regCount; i++) {
            values[i] = (response[3 + i * 2] << 8) | response[4 + i * 2];
        }
        
        return true;
    }
    
    bool testCommunication() {
        // Try to read voltage register (0x0000)
        uint16_t value;
        if (!readHoldingRegisters(0x0000, 1, &value)) {
            return false;
        }
        
        float voltage = value * 0.1f;
         Serial.printf(PSTR("  Test read - Voltage: %.1fV\n"), voltage);
        return true;
    }

};

}

#endif
