#include <Arduino.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include <cstring>

#include "wifi.h"
#include "device.h"
#include "utils.h"

namespace rpz{

#ifndef AP_SSID
#define AP_SSID "iot_edge"
#define AP_PWD ""
#endif

const PROGMEM char *page_tmpl = R"(
<!DOCTYPE html>
<html lang='en'>
    <head>
        <meta name='viewport' content='width=device-width'>
        <title>Rapidomize IoT Edge Setup</title>
    </head>
    <body style="display: flex;flex-direction: column;max-width: 600px;margin: auto;padding: 10px 20px;">
        <div style="display: flex;flex-direction: row;align-items: end; margin-bottom: 20px;">
            <img class="img-fluid" src="https://rapidomize.io/img/logo-text.svg" alt="Rapidomize | Low-Code Service Development Platform" loading="lazy" 
                style="height:auto;max-width: 100%; max-height: 56px;margin-right: 30px;">
            <h1>IoT Edge</h1>
        </div>
        <div>%s</div>
        <form action="/wifi" method="post" style="display: flex;flex-direction: column;">
            <h2>Available WiFi Networks</h2>
            <p>Select a WiFi network and provide it's credentials</p>
            <label style="border: 1px solid gray; border-radius: 6px; padding: 10px;">SSIDs:
                <div style="display: flex;flex-direction: column;">
                    %s
                </div>
            </label>
            <label style="margin-top: 10px;">Wifi password:
                <input type="password" name="pwd">
            </label>

            <h2>MQTT Broker</h2>
            <p>Specify MQTT Broker details. Supports MQTT 3.1.1 with QoS 0</p>
            <div style="display: flex;flex-direction: column; grid-gap: 5px;">
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">Host</span><input type="text" name="host" style="flex-grow: 1"></div>
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">Port</span><input type="number" name="port"  style="flex-grow: 1"></div>
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">Client ID</span><input type="text" name="clientId"  style="flex-grow: 1"></div>
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">Username</span><input type="text" name="username"  style="flex-grow: 1"></div>
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">Password</span><input type="password" name="password"  style="flex-grow: 1"></div>
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">Publishing Topic</span><input type="text" name="topic"  style="flex-grow: 1"></div>
                <div style="display: flex;flex-direction: row;"><span style="width: 120px;">TLS/SSL</span><input type="checkbox" name="tls" disabled></div>
                <div style="display: flex;flex-direction: row;">
                    <span style="width: 120px;">Version</span>
                    <select name="ver">
                        <option value="3.1.1">3.1.1</option>
                    </select>
                </div>
                <div style="display: flex;flex-direction: row;">
                    <span style="width: 120px;">QoS</span>
                    <select name="qos">
                        <option value="0">0</option>
                    </select>    
                </div>
            </div>
            <input type="submit" style="margin: 20px auto; width: 200px;height: 30px;">
        </form>
    </body>
</html>
)";

const PROGMEM char *frag_tmpl = R"(<div><input type="radio" name="ssid" value="%s"> <label>%s</label></div>)";
const PROGMEM char *err_tmpl = R"(<div style="color: red;">Err: %s</div>)";
const PROGMEM char *success_pg = R"(<!DOCTYPE html><html lang='en'>
      <head>
          <meta name='viewport' content='width=device-width'>
          <title>Rapidomize IoT Edge</title>
      </head>
      <body>Thanks</body></html>)";


void Wifi::homePage(int status, const char *err){
  Serial.println(F("home page..."));
  String pssid;
  char *fr = (char *) malloc(2048);
  for(int i=0;i<ssid_cnt && i < 20; i++){
    Serial.println(ssids[i].c_str());
    sprintf(fr, frag_tmpl, ssids[i].c_str(), ssids[i].c_str());
    pssid += fr;
  }

  fr[0] = '\0';
  if(err){
    sprintf(fr, err_tmpl, err);
  }
  //Serial.println(F("home page2..."));
  char *page = (char *) malloc(8192);
  
  sprintf(page, page_tmpl, fr, pssid.c_str());

  server.send(status, "text/html", page);
  //Serial.println(F("home page3..."));
  free(fr);
  free(page);
}

/* 
  Go to http://192.168.4.1 in a web browser to configure the gateway
*/
void Wifi::handleRoot() {
  Serial.println(F("handleRoot..."));
  homePage(200, NULL);  
}

//page submitted
void Wifi::onSetup(){
  Serial.println(F("\nSetting up WiFi..."));
  wifi_ssid.clear();
  wifi_pwd.clear();

  wifi_ssid = server.arg("ssid");
  wifi_pwd = server.arg("pwd");
  host = server.arg("host");
  port = server.arg("port").toInt();
  clientId = server.arg("clientId");
  username = server.arg("username");
  password = server.arg("password");
  topic = server.arg("topic");
  tls = server.arg("tls").toInt();

  if(wifi_ssid.length() != 0 && wifi_pwd.length() != 0){
    connectWiFi(true);

    if(host.length() != 0 && port > 0){
      server.send(200, "text/html", success_pg);
      delay(1000);
      connectMQTT(true);
    }else
      homePage(400, "Invalid MQTT Connection details!");  
      
  }else 
    homePage(400, "Invalid WiFi details!");  
}

void Wifi::connectWiFi(bool setup){
    if(WiFi.isConnected() && WiFi.SSID() == wifi_ssid) return;
    
    Serial.printf(PSTR("Connecting to WiFi ssid: %s, pwd: %s\n"), wifi_ssid.c_str(), wifi_pwd.c_str());
    WiFi.disconnect(true);  //disconnect from wifi AP to set wifi connection in STA
    //WiFi.mode(WIFI_AP_STA);    //init wifi mode
    WiFi.begin(wifi_ssid, wifi_pwd);
    int retry = 0;
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      if(WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_DISCONNECTED){
          if(retry++ > 100){
            Utils::buzzer(5);
            Serial.printf(PSTR("Invalid SSID/Password! %s!!!\n "), 
                WiFi.status() == WL_CONNECT_FAILED?"CONNECT_FAILED":WiFi.status() == WL_DISCONNECTED?"DISCONNECTED":"");
            if(setup)
              homePage(400, "Invalid SSID/Password!");
            return;
          }
      } 
      delay(500);
      Serial.printf(PSTR("%d, .. "), WiFi.status());
    }

    Serial.printf(PSTR("\nConnected to %s IP address: %s\n"), wifi_ssid.c_str(), WiFi.localIP().toString().c_str());
    Utils::buzzer(2);

    if (MDNS.begin("rapidomize")) {
      Serial.println(F("MDNS responder started"));
    }

    prefs->putString("wifi_ssid", wifi_ssid);
    prefs->putString("wifi_pwd", wifi_pwd);

    haswifi = true;
}

//connect to MQTT server
void Wifi::connectMQTT(bool setup){
    if(host.length() == 0 || port == 0){
      Serial.printf(PSTR("MQTT connection failed! Invalid Connection details: ssl://%s:%d\n"), host.c_str(), port);
      if(setup){
        homePage(400, "Invalid MQTT Connection details!");  
      }

      return;
    } 
    // sslClient.setFingerprint(finger_print);
    //sslClient.setTrustAnchors(&cert);
    //sslClient.setInsecure();
    if(wifiClient.connected())  wifiClient.flush();
    wifiClient.stop();
    Serial.printf(PSTR("Connecting to MQTT broker: ssl://%s:%d\n"), host.c_str(), port);

    int err;
    int count=0;
    while(!(err = wifiClient.connect(host.c_str(), port))){
      if (count++ > 100) {
          Utils::buzzer(5);
          Serial.printf(PSTR("MQTT connection failed! Error code = %d\n"),  err);
          if(setup){
            homePage(400, "Invalid MQTT Connection details!");  
          }
          //Serial.println(sslClient.getLastSSLError());
          return;
      }
    }
    
    Serial.println(F("connected to server"));

    //Utils::buzzer(2);
    prefs->putString("host", host);
    prefs->putShort("port", port);

    hasmqtt = true;
}

uint8_t Wifi::isConnected(){
  return wifiClient.connected();
}

void Wifi::handleAPReq(){
  server.handleClient();
}

void Wifi::scan() {
  
  bool hidden;
  Serial.println(F("Starting WiFi scan..."));

  ssid_cnt = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (ssid_cnt == 0) {
    Serial.println(F("No WIFI networks found"));
  } else if (ssid_cnt > 0) {
    Serial.printf(PSTR("%d WIFI networks found:\n"), ssid_cnt);
  
    // Print unsorted scan results
    for (int8_t i = 0; i < ssid_cnt && i < 20; i++) {
      ssids[i] = WiFi.SSID(i);
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", ssids[i].c_str());
      Serial.print(" | ");
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);

      yield();
    }
    Serial.println("-------------------------------------");
    WiFi.scanDelete();

  } else {
    Serial.printf(PSTR("WiFi scan error %d\n"), ssid_cnt);
  }

  // Wait a bit before scanning again
  //delay(5000);
}


void Wifi::init(Preferences *prefs) {
  this->prefs = prefs;

  if (!prefs->begin("iot_edge", false)) {
      Serial.println("Failed to initialize NVS. Restarting...");
      ESP.restart();
  }
  //prefs->putShort("edge_ver", 1);

  Serial.println(F("Init WiFi..."));
  WiFi.mode(WIFI_AP_STA);

  wifi_ssid = prefs->getString("wifi_ssid");
  wifi_pwd = prefs->getString("wifi_pwd");
  host = prefs->getString("host");
  port = prefs->getShort("port");
  clientId = prefs->getString("clientId");
  username = prefs->getString("username");
  password = prefs->getString("password");
  topic = prefs->getString("topic");
  tls = prefs->getShort("tls");
  /* ver = prefs->getBool("ver");
  qos = prefs->getUInt("qos"); */

  if(wifi_ssid.length() == 0 || wifi_pwd.length() == 0){
    WiFi.disconnect(); //if any previous connections?
    delay(100);
    wifi_ssid.clear();
    wifi_pwd.clear();

    Serial.printf(PSTR("Creating AP %s"), AP_SSID);
    WiFi.softAP(AP_SSID, NULL);//AP_PWD
    
    IPAddress myIP = WiFi.softAPIP();
    Serial.printf(PSTR(" IP address: %s\n"), myIP.toString().c_str());
  }else{
    connectWiFi();
    connectMQTT();
  }

  //provide page for config or reconfig
  scan();

  server.on("/", HTTP_GET, std::bind(&Wifi::handleRoot, this));
  server.on("/wifi", HTTP_POST, std::bind(&Wifi::onSetup, this));
  server.begin();
}

}