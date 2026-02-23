#ifndef RPZ_WIFI_H_
#define RPZ_WIFI_H_

#include <Preferences.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
// #include <WebServer.h>
#include <PubSubClient.h>

#include "peripheral.h"

namespace rpz{

//connection & config provider
class ConProvider{
    public: 
        ConProvider(): server(80), host("ics.rapidomize.com"), port(8883), events("/evts"){
            server.addHandler(&events);
        }

        void init(PubSubClient *mqttClient, Peripheral **peripherals, Preferences *prefs);
        bool connectMQTT(bool setup=false);
        bool connectWiFi(bool setup=false);
        void scan();

        void homePage(AsyncWebServerRequest *request, int status=200, const char *err=nullptr);
        void err(const char *err);
        char * getDash();
        char * getWifi();
        char * getMqtt();
        char * getPeri();
        void onWifi(AsyncWebServerRequest *request);
        void onMqtt(AsyncWebServerRequest *request);
        void onPeri(AsyncWebServerRequest *request);
        void onReqUpgrade();
        void onUpgrade();
        void toJson(AsyncWebServerRequest *request, JsonDocument &doc);

        void save();

        //mqtt
        String host; 
		int port; 
        String clientId;
        String username;
        String password;
        String topic;
        bool tls;
        String ver;
        uint8_t qos;

        bool haswifi = false;
        bool hasSetup = false;

        AsyncEventSource events;

    private:
        String ssids[20];
        uint8_t ssid_cnt;
        String wifi_ssid;
        String wifi_pwd;

        // WebServer server;
        AsyncWebServer server;
        WiFiClient wifiClient; 
        PubSubClient *mqttClient;
        Preferences *prefs;
        Peripheral **peripherals;
};

}

#endif  //RPZ_WIFI_H_