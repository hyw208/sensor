#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

//===================================
#include "Adafruit_VEML7700.h"
Adafruit_VEML7700 veml = Adafruit_VEML7700();
bool veml_detected = false;

//===================================
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
bool aht_detected = false;

//===================================
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
  //
  Serial.begin(115200);

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print("WiFi connection failed! Error code = ");
    Serial.println(WiFi.status());
    delay(5000);
  }
  Serial.print("You're connected to the WiFi network ");
  Serial.println(ssid);

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

  //===================================
  if (!veml.begin()) {
    Serial.print("Light sensor not found");
  } else {
    Serial.print("Light sensor found");
    veml_detected = true;
    veml.setGain(VEML7700_GAIN_1);
    veml.setIntegrationTime(VEML7700_IT_800MS);
    veml.setLowThreshold(10000);
    veml.setHighThreshold(20000);
    veml.interruptEnable(true);
  }
  
  //===================================
  if (!aht.begin()) {
    Serial.print("Temperature and humidity sensors not found");
  } else {
    Serial.print("Temperature and humidity sensors found");
    aht_detected = true;
  }
}

void loop() {
  if (!mqttClient.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        Serial.print("WiFi connection failed! Error code = ");
        Serial.println(WiFi.status());
        delay(5000);
      }
      Serial.print("You're connected to the WiFi network ");
      Serial.println(ssid);
    }
    while (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
      delay(5000);
    }
    Serial.println("You're connected to the MQTT broker!");
    Serial.println(); 
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
    
    //===================================
    // temp
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    float tf = (temp.temperature/5.0) * 9 + 32;

    // humidity
    float hm = humidity.relative_humidity;
    
    // lumen
    float l = veml.readLux();    
    
    //===================================
    DynamicJsonDocument doc(256); 
    doc["client"] = clientId;
    doc["temp"]= tf;
    doc["humidity"] = hm;
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
