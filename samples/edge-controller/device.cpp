#include <Arduino.h>

#include <string>
#include <functional>
#include <memory>

#include "device.h"
#include "module.h"
#include "sensor.h"
#include "utils.h"

#include <ArduinoJson.h>


#define LED 2
#define BUZZER 12

rpz::Device device;

namespace rpz{


const PROGMEM char *STATE_MSG = R"({"msg":{"_evt":"circuit", "state":%d},"op":2})";
//FIXME: format
const PROGMEM char *ACK_MSG = R"({"msg":{"n":"circuit", "v":%d},"op":2})";

void Device::init(){
    wifi.init(&prefs);

    mqttClient.setClient(wifi.wifiClient);

    Serial.println(F("IoT Edge is ready to connect"));

    alarms[0] = false; alarms[1]=false;

    pinMode(LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    //i2c_master_init();
    sensors.init();
}


bool Device::connect(){
    if(!mqttClient.connected()){
        Serial.printf(PSTR("\nConnecting to MQTT broker: ssl://%s:%d, with ClientID: %s Username: %s Password: %s topic: %s \n"), 
            wifi.host.c_str(), wifi.port, wifi.clientId.c_str(), wifi.username.c_str(), wifi.password.c_str(), wifi.topic.c_str());

        if(!wifi.isConnected())  wifi.connectMQTT();

        if(mqttClient.connect(wifi.clientId.c_str(), wifi.username.c_str(), wifi.password.c_str())){//mqttClient.connect().connect(HOST, PORT)){
            Utils::buzzer(2);
           
            // subscribe to a topic:
            //Serial.printf(PSTR("Subscribing to topic: %s"), subtopic);
            //mqttClient.subscribe(subtopic);
            prefs.putString("clientId", wifi.clientId);
            prefs.putString("username", wifi.username);
            prefs.putString("password", wifi.password);
            prefs.putString("topic", wifi.topic);
            prefs.putShort("tls", wifi.tls);
            return true;
        }else
            Serial.printf(PSTR("Failed connecting to MQTT broker: ssl://%s:%d\n"), wifi.host.c_str(), wifi.port);
    }
    return true;
}

//we should create topic & payload copy of the values if they are required after the callback returns
void Device::on_msg(const char* topic, byte* payload, unsigned int length) {
    // we received a message, print out the topic and contents
    Serial.printf(PSTR("Received topic: %s, len: %d, payload: %s\n"), topic, length, payload);

    //{"msg":{"n":"circuit","v":1,"dt":"boolean","rwx":"","op":1},"mid":3,"op":1,"uri":"1092720020131081d33f4-7-251a5cdc6b9df"}
    JsonDocument doc;
    auto err = deserializeJson(doc, payload);
    if(err){
        return;
    }
    
    
    JsonObject msg = doc["msg"];
    Serial.printf("msg => %s\n", (const char* )msg["n"]);
    int op = (int) doc["op"];
    Serial.printf("op: %d\n", op);
    switch (op){
        case RPZ_READ: break;
        case RPZ_WRITE:{
            auto v = 10;//msg["v"];
            const char* dt = (const char* )msg["dt"];
            Serial.printf("dt: %s\n", dt);

            if(strcmp(dt, "boolean") == 0){
                Serial.printf("vb: %d\n", (int)v);
            }else if(strcmp(dt, "double") == 0){
                Serial.printf("vd: %f\n", (double)v);
            }else if(strcmp(dt, "integer") == 0){
                Serial.printf("vi: %d\n", (long)v);
            }
            if(v == 0){
                device.circuit = false;
                // digitalWrite(D3, HIGH);
            }else{
                device.circuit = true;
                // digitalWrite(D3, LOW);
            }
        } break;
        case RPZ_EXEC: break;
        
        default:
            break;
    }
    const char* uri = doc["uri"];
       
    char acktopic[256];
    sprintf(acktopic, "ack", uri);
    char jmsg[128];
    sprintf(jmsg, ACK_MSG, 0);
    device.send(jmsg, acktopic);
}

void Device::send(const char* msg, const char* topic) {
    
    if (mqttClient.connected()) {
        Serial.printf(PSTR("\nmsg: %s\n"), msg);

        mqttClient.publish(topic, msg);
    }else
        Serial.println(F("not connected"));
}

void Device::update(){
    wifi.handleAPReq();
    if(!wifi.hasmqtt){
        Serial.print(".");
    }else{
        Serial.print(".");
        if(connect()){
            mqttClient.loop();

            const unsigned long now = millis();
            if (now - prev >= interval) {
                // save the last time a message was sent
                prev = now;

                char * data = sensors.read();
                if(data != nullptr)
                    send(data, wifi.topic.c_str());
                
            }
        }
    }
}

}

