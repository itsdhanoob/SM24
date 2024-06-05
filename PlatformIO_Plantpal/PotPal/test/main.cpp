#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

const char* ssid = "PlantPal-S2";        // Replace with your Wi-Fi SSID
const char* password = "78847884"; // Replace with your Wi-Fi password

const int ledPin = 15;
const int blinkInterval = 100; // Interval in milliseconds for blinking


uint8_t thisMacAddress[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

// Callback function to handle received data
void onDataReceived(const uint8_t* mac, const uint8_t* data, int len) {
    Serial.print("Received from MAC: ");
    for (int i = 0; i < 6; ++i) {
        Serial.print(mac[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.print(", Data: ");
    for (int i = 0; i < len; ++i) {
        Serial.print((char)data[i]);
    }
    Serial.println();
}

void initEspNow()
{
    WiFi.mode(WIFI_STA);
  WiFi.disconnect();

      if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
}


void setup() {
  // Set up LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Initially turn off the LED
  digitalWrite(ledPin, LOW);

  // Start connecting to Wi-Fi
  WiFi.begin(ssid, password);
}

void loop() {
  // Check Wi-Fi connection status
  if (WiFi.status() == WL_CONNECTED) {
    // Connected: Turn on the LED
    digitalWrite(ledPin, HIGH);
  } else {
    // Not connected: Blink the LED
    static unsigned long lastBlinkTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastBlinkTime >= blinkInterval) {
      // Toggle the LED state
      digitalWrite(ledPin, !digitalRead(ledPin));
      lastBlinkTime = currentTime;
    }
  }

  // Small delay to avoid potential issues
  delay(10);
}