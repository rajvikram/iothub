#include <SPI.h>
#include <Wire.h>

#define LOOP_DELAY 1000

const int AirValue = 644;   //you need to replace this value with Value_1
const int WaterValue = 368;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
 
 
void setup() {
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps;
}
 
 
void loop() 
{
soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

// Clamp percent values
if(soilmoisturepercent > 100) { soilmoisturepercent = 100; }
else if(soilmoisturepercent <0){ soilmoisturepercent = 0; }

Serial.printf("\nSoil moisture [%d]: %d %%", soilMoistureValue, soilmoisturepercent);
Serial.println();

delay(LOOP_DELAY);
}
