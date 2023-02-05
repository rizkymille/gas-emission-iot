#include "max6675.h"

// Thermocouple sensor params
#define MAX6675_CS 10
#define MAX6675_MISO 12
#define MAX6675_SCLK 13

MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);

void setup() {
  Serial.begin(115200);
}

void loop() {
  
  if (Serial.available() > 0) {
    String recieve = Serial.readString();
    if(recieve.equals("G")) {
      static float temp = thermocouple.readCelsius();
      delay(100);
      Serial.print(temp);
    }
  }
}
