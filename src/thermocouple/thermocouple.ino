/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include "Adafruit_MAX31856.h"
#include "InfluxDb.h"

#ifndef DEVSSID
#define DEVSSID       "your-ssid"
#define DEVPSK        "your-password"
#endif

#define INFLUXDB_URL  "http://192.168.2.120:8086"

// How frequently will this device update it's data  
#define LOOP_DELAY     600e6  // For deep sleep this is in microsecs

const char* ssid     = DEVSSID;
const char* ssid_pass = DEVPSK;

const char* influxdb_url = INFLUXDB_URL;
const char* influxdb_db = "iot";

WiFiClient net;
// Single InfluxDB instance
InfluxDBClient influxdb_client(influxdb_url, influxdb_db);

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

  // make sure we are connected to Wifi
  connect();

  float cjTemp, hjTemp;

  // Get both cold junction and thermocouple temperatures
  cjTemp = cToF(maxim.readCJTemperature());
  hjTemp = cToF(maxim.readThermocoupleTemperature());
  Serial.printf("\nCold Junction Temp: %f, Thermocouple Temp : %f", cjTemp, hjTemp);

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

  // Now that we have the data, send it to influxdb
  Point deviceData("temperature");
  // Set tags
  deviceData.addTag("device", "solar_oven");
  
  // Add data
  deviceData.addField("cold", cjTemp);
  deviceData.addField("hot", hjTemp);
  
  // Write data
  influxdb_client.writePoint(deviceData);

  Serial.println("");
  // Wait for bit before updating again
  //delay(LOOP_DELAY);
  ESP.deepSleep(LOOP_DELAY);


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
}
