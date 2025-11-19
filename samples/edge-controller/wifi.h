#ifndef RPZ_WIFI_H_
#define RPZ_WIFI_H_

#include "WiFi.h"
#include <WebServer.h>
#include <Preferences.h>

namespace rpz{


class Wifi{
    public: 
        Wifi(): server(80), host("ics.rapidomize.com"), port(8883){}

        void init(Preferences *prefs);
        void connectWiFi(bool setup=false);
        void connectMQTT(bool setup=false);
        void scan();
        void handleAPReq();
        void handleRoot();
        void homePage(int status, const char *err);
        void onSetup();
        uint8_t isConnected();

        WiFiClient wifiClient; 
        //mqtt
        String host; 
		int port; 
        String clientId;
        String username;
        String password;
        String topic;
        int tls;
        String ver;
        int qos;

        bool haswifi = false;
        bool hasmqtt = false;

    private:
        String ssids[20];
        int ssid_cnt;
        String wifi_ssid;
        String wifi_pwd;
        

        WebServer server;
        Preferences *prefs;
};

}

#endif  //RPZ_WIFI_H_