#include "SerialTransfer.h"
#include "max6675.h"

// Thermocouple sensor params
#define MAX6675_CS 10
#define MAX6675_MISO 12
#define MAX6675_SCLK 13

SerialTransfer serialParser;
MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);

uint16_t recSize, sendSize;
char command;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  serialParser.begin(Serial);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (serialParser.available()) {
    recSize = 0;
    recSize = serialParser.rxObj(command, recSize);

    if(command == "G") {
      sendSize = serialParser.txObj(thermocouple.readCelsius(), sendSize);
      serialParser.sendData(sendSize);
      sendSize = 0;
    }
    else if(command == "R") {
      // TO DO: set nano to deep sleep mode, and RX to interrupt mode
    }
  }

}
