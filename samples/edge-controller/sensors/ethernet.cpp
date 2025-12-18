/*MQTT control relay is implemented using  Ethernet communication*/
#include <ETH.h> 
#include <WiFiUdp.h> 
#include<PubSubClient.h>

// Define the Ethernet address
#define ETH_ADDR        0  // Define the Ethernet power pin
#define ETH_POWER_PIN  -1  // Define the Ethernet MDC pin
#define ETH_MDC_PIN    23  // Define the Ethernet MDIO pin
#define ETH_MDIO_PIN   18  // Define the Ethernet type
#define ETH_TYPE       ETH_PHY_LAN8720 // Define the Ethernet clock mode
#define ETH_CLK_MODE   ETH_CLOCK_GPIO17_OUT


IPAddress local_ip(192, 168, 50, 200); // IP address for local 
IPAddress gateway(192, 168, 50, 1);// IP address for gateway
IPAddress subnet(255, 255, 255, 0);// IP address for subnet
IPAddress dns(192, 168, 50, 1);// IP address for DNS

const char* mqttServer ="192.168.50.101";// this is your MQTT server 
const int mqttPort=1883;
const char* mqttUsername="mqtt";  // this is your MQTT server username（Optional)
const char* mqttPassword="123"; // this is your MQTT server password（Optional)


WiFiClient espClient; // creat an instance
PubSubClient client(espClient);//creat an instance

void setup(){
    Serial.begin(115200);
    Serial.println(__FILE__);// print the folder path
   String topic = "relay_control/command"; // creat the topic

  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);    // Initialize the Ethernet
  if (ETH.config(local_ip, gateway, subnet, dns, dns) == false) {   // If Ethernet configuration fails
   Serial.println("LAN8720 Configuration failed."); // Print configuration failed message
  }else{// Else print configuration success message
    Serial.println("LAN8720 Configuration success.");}
  Serial.println("Connected");// Print connected message
  Serial.print("IP Address:"); // Print the local IP address
  Serial.println(ETH.localIP());


  // Try to connect to MQTT SERVER
    client.setServer(mqttServer,mqttPort);
    client.setCallback(callback);
  while(!client.connected()){
      Serial.println("Connecting to MQTT...");
      if(client.connect("edge-controller",mqttUsername,mqttPassword)){
        Serial.println("MQTT Connected");
      }else {
        Serial.println("Check your MQTT settings");
        delay(2000);

      }
  }
  pinMode(2,OUTPUT);
  pinMode(15,OUTPUT);
   
  Serial.println(topic.c_str());
  client.subscribe(topic.c_str());

}
void loop(){
    client.loop();
}
void callback(char* topic,byte* payload,int length) {
  Serial.println("Message arrived in topic:"+ String(topic));
  Serial.print("Payload is:");
  for(unsigned int i=0;i<length;i++) {
    Serial.write(payload[i]);
  }
  Serial.println();
  
  if(!strncmp((char*)payload,"{\"relay1\":\"on\"}",length)){  //  Payload {"relay1":"on"} will turn on relay1
     digitalWrite(2,HIGH);
  }
else if (!strncmp((char*)payload,"{\"relay1\":\"off\"}",length)){   //Payload {"relay1":"off"} will turn off relay1
  digitalWrite(2,LOW);
}

if (!strncmp((char *)payload, "{\"relay2\":\"on\"}", length)) {  //Payload {"relay2":"on"} will turn on relay2
  digitalWrite(15,HIGH);
}
else if (!strncmp((char *)payload, "{\"relay2\":\"off\"}", length)) { //Payload{"relay2":"off"} will turn off relay2
  digitalWrite(15,LOW);
  }
if (!strncmp((char *)payload, "{\"relay\":\"on\"}", length)) {  //Payload {"relay":"on"} will turn on  relay1 and relay2
  digitalWrite(2,HIGH);
  delay(200);
  digitalWrite(15,HIGH);
  delay(200);
  
}
 else if (!strncmp((char *)payload, "{\"relay\":\"off\"}", length)) {//Payload {"relay":"off"} will turn off  relay1 and relay2
  digitalWrite(2,LOW);
  delay(200);
  digitalWrite(15,LOW);
  delay(200);
  }
}
