#include <Arduino.h>

#include "device.h"
#include "utils.h"
#include <string>

extern rpz::Device device;

//just an indicator to show device is alive.
int indicate = 0;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println(F("init()"));

    device.init();
}

void loop() {
    if(indicate++ > 20){ 
        Utils::indicate();
        indicate = 0;
    }
    device.update();

    delay(1000);
}


