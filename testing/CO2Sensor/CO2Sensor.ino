#include <MG811.h>

#include <SPI.h>

#define CO2_PIN 33 // GPIO ESP32
#define CALIB_PIN 4 // GPIO ESP32

#define v400 4.535
#define v40000 3.206

MG811 CO2_sens(CO2_PIN);

int calib_counter;

void setup(){
  CO2_sens.begin(v400, v40000);
  pinMode(CALIB_PIN, INPUT);
  
  Serial.begin(9600); // serial monitor

  // Enter calibration mode
  while(digitalRead(CALIB_PIN) == HIGH) {
    calib_counter++;
    delay(1000);
  }

  if(calib_counter == 3) {
    CO2_sens.calibrate();
  }
}

void loop(){
  //Print CO2 concentration
  Serial.print("CO2 Concentration: ");
  Serial.print(CO2_sens.read());
  Serial.println("ppm");
  
  delay(2000);
}
