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
const char *sensor_topic = "/plantpal/sensors";
const char *config_topic = "/plantpal/config";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Start connecting to Wi-Fi
  WiFi.begin(ssid, password);

  uint16_t counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 5000) {
    counter++;
    if (counter % 10 == 0) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    delay(1);
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
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
      // Once connected, publish an "alive" message
      client.publish("/plantpal/alive", "ESP32 is alive");
      // Subscribe to the configuration topic
      client.subscribe(config_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
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
  doc["soilMoisture"] = soilMoisture;
  doc["freeHeap"] = esp_get_free_heap_size();
  

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  client.publish(sensor_topic, jsonBuffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Handle the configuration message if needed
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish sensor data every 10 seconds
  static unsigned long lastPublishTime = 0;
  if (millis() - lastPublishTime > 10000) {
    publishSensorData();
    lastPublishTime = millis();
  }
}
