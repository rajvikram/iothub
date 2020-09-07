/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include "Adafruit_MAX31856.h"

#ifndef DEVSSID
#define DEVSSID "your-ssid"
#define DEVPSK  "your-password"
#endif

const char* ssid     = DEVSSID;
const char* ssid_pass = DEVPSK;
const char* iothub_addr = "192.168.2.120"
const int iothub_influx_port = 8086

WiFiClient net;

unsigned long lastMillis = 0;

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31856 maxim = Adafruit_MAX31856(15, 13, 12, 14);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31856 maxim = Adafruit_MAX31856(15);

void setup() {
  Serial.begin(115200);

   WiFi.begin(ssid, ssid_pass);

  Serial.println("MAX31856 thermocouple test");
  maxim.begin();

  maxim.setThermocoupleType(MAX31856_TCTYPE_K);
}

void loop() {

  float cjTemp, thTemp;

  // Get both cold junction and thermocouple temperatures
  cjTemp = cToF(maxim.readCJTemperature());
  thTemp = cToF(maxim.readThermocoupleTemperature());
  Serial.printf("\nCold Junction Temp: %f, Thermocouple Temp : %f", cjTemp, thTemp);

  // Check and print any faults
  uint8_t fault = maxim.readFault();
  if (fault) {
    if (fault & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
    if (fault & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
    if (fault & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
    if (fault & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
    if (fault & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
    if (fault & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
    if (fault & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
    if (fault & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
  }
  delay(5000);

//  client.loop();
//  delay(10);  // <- fixes some issues with WiFi stability
//
//  if (!client.connected()) {
//    connect();
//  }
//
//  // publish a message roughly every second.
//  if (millis() - lastMillis > 1000) {
//    lastMillis = millis();
//    client.publish("/temperature", String(maxim.readThermocoupleTemperature()));
//  }
}

float cToF(float c) {
  if (isnan(c)) {
    return c; // can't convert it
  }
  return (9.0 / 5.0) * c + 32.0;
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

//  Serial.print("\nconnecting...");
//  while (!client.connect("admin", "admin", "admin")) {
//    Serial.print(".");
//    delay(1000);
//  }
//
//  Serial.println("\nconnected!");
//
//  client.subscribe("/temperature");
  // client.unsubscribe("/temperature");
}
