#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
  #include <ESP8266WiFi.h>
#endif
#include <ArduinoJson.h>
#include "arduino_secrets.h"

#define T_PIN A1
#define L_PIN A3

char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "IP_ADDRESS"; // broker/server ip, 156 is mac air, 180 is mac desktop
int        port     = 1883;
const char topic[]  = "sensor/json";

const char clientId[] = "mrk-1010-g1";

const long interval = 10000;

unsigned long previousMillis = 0;
int count = 0;

void setup() {
  //TO BE COMMENT OUT
  //Initialize serial and wait for port to open:
//  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId(clientId); // ** can't get it from python subscriber

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  while (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(5000);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  if (!mqttClient.connected()) {
    while (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
      delay(5000);
    }    
  } else {
    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    mqttClient.poll();
  }

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    
//    mqttClient.print("hello ");
//    mqttClient.print(count);

//===================================
    // temp
    float v = analogRead(T_PIN) * (5000/1024.0);
    float t = v / 10;
    
    // lumen
    float l = analogRead(L_PIN);    
    
//===================================
    DynamicJsonDocument doc(256); 
    doc["client"] = clientId;
    doc["temp"]= t;
    doc["lumen"]= l;
    
    char json[256];
    serializeJson(doc, json);

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(json);
//===================================
    mqttClient.print(json);
    
    mqttClient.endMessage();

    Serial.println();

    count++;
  }
}
