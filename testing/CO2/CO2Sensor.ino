#include <MG811.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CO2_PIN 33 // GPIO ESP32
#define v400 4.535
#define v40000 3.206

MG811 CO2_sens = MG811(CO2_PIN);

void setup(){
  CO2_sens.begin(v400, v40000);
  /*
   * Calibration mode
   * pinMode(CALIB_PIN, INPUT);
   * if(digitalRead(CALIB_PIN) == HIGH) {
   *  CO2_sens.calibrate();
   * }
   */
  Serial.begin(9600); // serial monitor
}

void loop(){
  //Print CO2 concentration
  Serial.print("CO2 Concentration: ");
  Serial.print(CO2_sens.read());
  Serial.println("ppm");
  
  delay(2000);
}
