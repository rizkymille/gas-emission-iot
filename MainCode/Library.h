// PIN CO SENSOR

int alarm = 0; 
float sensor_volt; 
float RS_gas; 
float ratio; 
//-Replace the name "R0" with the value of R0 in the demo of First Test -/ 
float R0 = 0.91; 
 
int sensorValue = analogRead(A1);  //PIN SENSOR CO2 = A1

//PIN TEMPERATURE SENSOR

#include "max6675.h"
int thermoDO = 4;
int thermoCS = 5; 
int thermoCLK = 6;

//PIN CO2 SENSOR
#include <MG811.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define CO2_PIN 33 // GPIO ESP32
#define v400 4.535
#define v40000 3.206
MG811 CO2_sens = MG811(CO2_PIN);

  
