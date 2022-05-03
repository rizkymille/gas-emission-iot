#include "max6675.h"

#define MAX6675_CS 15
#define MAX6675_MISO 0
#define MAX6675_SCLK 5

MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);

void setup() {
  Serial.begin(57600);
}

void loop() {
  Serial.print("Temp:");
  Serial.println(thermocouple.readCelsius());
  delay(1000);
}
