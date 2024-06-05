#include <Arduino.h>

#include <omegaWireless.h>
#include <omegaNOW.h>

const char *ssid = "PlantPal-S2";  // Replace with your Wi-Fi SSID
const char *password = "78847884"; // Replace with your Wi-Fi password
uint8_t counter;
const int ledPin = 15;
const int blinkInterval = 100; // Interval in milliseconds for blinking
// omegaWireless manager;
ulong prevMilLed = 0;
ulong prevMil = 0;
ulong retrytimems = 1000;

omegaWireless wirelessManager = omegaWireless("PotPal");

void setup()
{
  // Set up LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 0);

  uint16_t counter = 0;
  wirelessManager.connect(ssid, password);

  while (WiFi.status() != WL_CONNECTED && counter > 5000)
  {
    /* code */
    counter++;
    if (counter % 10 == 0)
      digitalWrite(15, !digitalRead(15));

    delay(1);
  }

  while (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(15, !digitalRead(15));
    delay(500);
  }

  digitalWrite(15, 1);
  wirelessManager.initESPNOW(false);
}

void loop()
{

  delay(10);
}
