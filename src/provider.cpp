#include <Arduino.h>
// #include <ESPmDNS.h>

#include <cstring>

#include "provider.h"
#include "device.h"
#include "utils.h"
#include "tmpl.h"

namespace rpz{

#ifndef AP_SSID
#define AP_SSID "iot_edge"
#define AP_PWD ""
#endif


/* 
  Go to http://192.168.4.1 in a web browser to configure the gateway
*/      
void ConProvider::homePage(AsyncWebServerRequest *request, int status, const char *err) {
  Serial.printf(PSTR("Home page - %d - %s\n"), status, err?err:"");
  
  char *dash = getDash();
  char *wifi = getWifi();
  char *mqtt = getMqtt();
  char *peri = getPeri();

  char *page = (char *) malloc(8192*4);
  if(wifi_ssid.length() == 0 || wifi_pwd.length() == 0){
    sprintf(page, main_tmpl, err?err:"", dash, wifi, "", "");
  }else{
    sprintf(page, main_tmpl, err?err:"", dash, wifi, mqtt, peri);
  }  

  request->send(status, "text/html", page);

  free(dash);
  free(wifi);
  free(mqtt);
  free(peri);
  free(page);
}

//page submitted
char *ConProvider::getDash(){
  char *fr = (char *) malloc(4096);
  sprintf(fr, dash_tmpl, "rpz-d2x2t2ux-we", RPZ_VERSION, ESP.getCpuFreqMHz(), ESP.getSketchSize()/1024, "");
  return fr;
}

char * ConProvider::getWifi(){
  String pssid;
  char *fr = (char *) malloc(4096);
  for(int i=0;i<ssid_cnt && i < 20; i++){
    Serial.println(ssids[i].c_str());
    sprintf(fr, ssid_tmpl, ssids[i].c_str(), (wifi_ssid.equals(ssids[i]))?"checked":"", ssids[i].c_str());
    pssid += fr;
  }

  fr[0]='\0';
  sprintf(fr, wifi_tmpl, pssid.c_str());
  return fr;
}

char * ConProvider::getMqtt(){
  char *fr = (char *) malloc(4096);
  sprintf(fr, mqtt_tmpl, host, port, clientId, username, "xxxxxxxx", topic, tls?"checked":"", ver, qos);
  return fr;
}

char *ConProvider::getPeri(){
  String peri;
  
  for(int i=0; i < MAX_PERIPHERALS; i++){
    char *fr = peripherals[i]->confpg();
    peri += fr;
    free(fr);
  }
  char *fr = (char *) malloc(4096*2);
  sprintf(fr, peri_tmpl, peri.c_str());
  return fr;
}

void ConProvider::onWifi(AsyncWebServerRequest *request){
  Serial.println(F("\nSetting up WiFi..."));
  wifi_ssid.clear();
  wifi_pwd.clear();

  JsonDocument doc;
  toJson(request, doc);

  wifi_ssid = (const char *)doc["ssid"];
  wifi_pwd = (const char *)doc["pwd"];

  if(wifi_ssid.length() != 0 && wifi_pwd.length() != 0){
    if(connectWiFi(true)){
      request->send(200, "application/json", "{}");
      return;
    }     
  }
  request->send(400, "application/json", "{\"err\":\"Error connecting to WiFi! Check SSID/Password!\"}");
  //homePage(request, 400, "Error connecting to WiFi! Check SSID/Password!");
}

void ConProvider::onMqtt(AsyncWebServerRequest *request){
  Serial.println(F("\nSetting up MQTT..."));

  JsonDocument doc;
  toJson(request, doc);

  host = (const char *)doc["host"];
  port = (int)doc["port"];
  clientId = (const char *)doc["clientId"];
  username = (const char *)doc["username"];
  password = (const char *)doc["password"];
  topic = (const char *)doc["topic"];
  tls = 1;//(char *)doc["tls").toInt();
  ver = (const char *)doc["ver"];
  qos = (int)doc["qos"];

  if(host.length() != 0 && port > 0 && clientId.length() != 0 
       && username.length() != 0 && password.length() != 0
       && topic.length() != 0 && tls >= 0
       && connectMQTT(true)){
      save();
      request->send(200, "application/json", "{}");
      return;
  } 
  request->send(400, "application/json", "{\"err\":\"Invalid MQTT Connection details!\"}");
}

void ConProvider::onPeri(AsyncWebServerRequest *request){
  Serial.println(F("\nSetting up Peripherals..."));

  JsonDocument doc;
  toJson(request, doc);

  for(int i=0; i < MAX_PERIPHERALS; i++){
    peripherals[i]->init(&doc);
  }

  request->send(200, "application/json", "{}");
}

void ConProvider::onReqUpgrade(){
  
}

void ConProvider::onUpgrade(){
}

void ConProvider::scan() {
  
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

void ConProvider::save(){
  //Utils::buzzer(2);
  prefs->putString("host", host);
  prefs->putShort("port", port);
  prefs->putString("clientId", clientId);
  prefs->putString("username", username);
  prefs->putString("password", password);
  prefs->putString("topic", topic);
  prefs->putShort("tls", tls);
}

bool ConProvider::connectWiFi(bool setup){
    if(WiFi.isConnected() && WiFi.SSID() == wifi_ssid) return false;
    
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
            return false;
          }
      } 
      delay(500);
      Serial.printf(PSTR("%d, .. "), WiFi.status());
    }

    Serial.printf(PSTR("\nConnected to %s IP address: %s\n"), wifi_ssid.c_str(), WiFi.localIP().toString().c_str());
    Utils::buzzer(2);

    /* if (MDNS.begin("rapidomize")) {
      Serial.println(F("MDNS responder started"));
    } */

    prefs->putString("wifi_ssid", wifi_ssid);
    prefs->putString("wifi_pwd", wifi_pwd);

    haswifi = true;
    return true;
}

//connect to MQTT server
bool ConProvider::connectMQTT(bool setup){
    if(host.length() == 0 || port == 0){
      Serial.printf(PSTR("MQTT connection failed! Invalid Connection details: ssl://%s:%d\n"), host.c_str(), port);
      return false;
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
      Serial.print("?");
      if (count++ > 5) {
          Utils::buzzer(5);
          Serial.printf(PSTR("\nMQTT connection failed! Error code = %d\n"),  err);
          //Serial.println(sslClient.getLastSSLError());
          return false;
      }
    }
    
    Serial.println(F("Connected to MQTT broker"));

    mqttClient->setClient(wifiClient);
    if(!mqttClient->connected()){
        Serial.printf(PSTR("\nConnecting to MQTT broker: ssl://%s:%d, with ClientID: %s Username: %s Password: %s topic: %s \n"), 
            host.c_str(), port, clientId.c_str(), username.c_str(), password.c_str(), topic.c_str());

        if(mqttClient->connect(clientId.c_str(), username.c_str(), password.c_str())){//mqttClient->connect().connect(HOST, PORT)){
            Utils::buzzer(2);
           
            // subscribe to a topic:
            //Serial.printf(PSTR("Subscribing to topic: %s"), subtopic);
            //mqttClient->subscribe(subtopic);
        }else{
            Utils::buzzer(5);
            Serial.printf(PSTR("Failed connecting to MQTT broker: ssl://%s:%d\n"), host.c_str(), port);
            return false;
        }
    }

    hasSetup = true;
    return true;
}

void ConProvider::toJson(AsyncWebServerRequest *request, JsonDocument &doc){
    String *data = (String *)request->_tempObject;

    if (!data) {
      request->send(400);
      return;
    }

    // no data ?
    if (!data->length()) {
      delete data;
      request->_tempObject = nullptr;
      request->send(400);
      return;
    }

    // deserialize and check for errors
    if (deserializeJson(doc, *data)) {
      delete data;
      request->_tempObject = nullptr;
      request->send(400);
      return;
    }

    Serial.printf(PSTR("jdata: %s\n"), (*data).c_str());
    delete data;
    request->_tempObject = nullptr;
}

void ConProvider::init(PubSubClient *mqttClient, Peripheral **peripherals, Preferences *prefs){

  this->mqttClient = mqttClient;
  this->peripherals = peripherals;
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
  tls = true;//prefs->getBool("tls");
  ver = prefs->getString("ver");
  qos = prefs->getUInt("qos"); 

  //provide page for config or reconfig
  scan();

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
    connectMQTT(true);
  }

  auto aggregator = [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (!index) {
        request->_tempObject = new String();
        ((String *)request->_tempObject)->reserve(total);
        // set timeout 30s
        request->client()->setRxTimeout(30);
      }
      if(!request->_tempObject) request->_tempObject = new String();
      ((String *)request->_tempObject)->concat((const char *)data, len);
  };

  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    this->homePage(request);
  });
  server.on("/wifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
    this->onWifi(request);
  }, NULL, aggregator);
  server.on("/mqtt", HTTP_POST, [this](AsyncWebServerRequest *request) {
    this->onMqtt(request);
  }, NULL, aggregator);
  server.on("/peri", HTTP_POST, [this](AsyncWebServerRequest *request) {
    this->onPeri(request);
  }, NULL, aggregator);
  // TODO:
  server.on("/upgrade", HTTP_GET, [this](AsyncWebServerRequest *request) {
    this->onReqUpgrade();
  });
  server.on("/upgrade", HTTP_POST, [this](AsyncWebServerRequest *request) {
    this->onUpgrade();
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    //send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!",NULL,millis(),1000);
  });

  server.addHandler(&events);

  server.begin();
}

}