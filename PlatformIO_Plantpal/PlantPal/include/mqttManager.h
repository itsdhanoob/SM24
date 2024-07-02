#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Include ArduinoJson library

#define ID 1
uint8_t hi = (uint8_t)'P';
uint8_t counter = 1;
uint16_t id = (hi << 8) | (counter & 0xFF);

#define LED_PIN 15
#define MQTT_PASSWORD "49937025" // Define the MQTT password here

const char *ssid = "OMEGA";  // Replace with your Wi-Fi SSID
const char *password = "49937025"; // Replace with your Wi-Fi password
const char *mqtt_server = "192.168.2.5";
const char *mqtt_user = "containership";
const char *mqtt_pass = MQTT_PASSWORD;

const char *alive_topic = "plantpal/alive/#";
const char *sensor_topic = "plantpal/sensors/#";
const char *config_topic = "plantpal/config/#";

WiFiClient espClient;
PubSubClient client(espClient);



typedef struct sensorDataPacket{
    uint16_t id;
    uint8_t tempc;
    uint8_t hum;
    uint8_t light;
    uint8_t moist;

};

sensorDataPacket curData;

void setup_wifi() {


  // Start connecting to Wi-Fi
  WiFi.begin(ssid, password);

  uint16_t counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 5000) {
    counter++;
    if (counter % 10 == 0) {
     
    }
    delay(1);
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      delay(1000);
      // Once connected, publish an "alive" message
      client.publish(alive_topic, "PlantPal is alive");
      // Subscribe to the configuration topic
      client.subscribe(config_topic);
      client.subscribe(sensor_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void publishSensorData() {
  StaticJsonDocument<256> doc;
  // Assuming you have functions to get the actual sensor values
  float temperature = 25.6;  // Replace with actual function
  float humidity = 60.2;     // Replace with actual function
  int soilMoisture = 70;     // Replace with actual function
  doc["id"] = id;
  doc["temperature"] = round(temperature);
  doc["humidity"] = round(humidity);
  doc["soilmoisture"] = soilMoisture;
  doc["freeHeap"] = esp_get_free_heap_size();
  

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  client.publish(sensor_topic, jsonBuffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
 /* Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Handle the configuration message if needed
 StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload, length);
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.f_str());
            return;
        }

        // Example: Extract values from the parsed JSON document
        if (doc.containsKey("id")) {
            curData.id = doc["command"].as<uint16_t>();
        }



*/
}
#endif