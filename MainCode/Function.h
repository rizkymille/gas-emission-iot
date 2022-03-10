#include "Library.h"

// CO SETUP DAN LOOP

void CO_setup() { 
  Serial.begin(9600); 
 
} 
 
void CO_loop() { 
 
  int alarm = 0; 
  float sensor_volt; 
  float RS_gas; 
  float ratio; 
 //-Replace the name "R0" with the value of R0 in the demo of First Test -/ 
  float R0 = 0.91; 
 
  int sensorValue = analogRead(A1);  //PIN SENSOR CO2 = A1
  sensor_volt = ((float)sensorValue / 1024) * 5.0; 
 RS_gas = (5.0 - sensor_volt) / sensor_volt; // Depend on RL on yor module 
 
 
  ratio = RS_gas / R0; // ratio = RS/R0 
 //------------------------------------------------------------/ 
 
  Serial.print("sensor_volt = "); 
  Serial.println(sensor_volt); 
  Serial.print("RS_ratio = "); 
  Serial.println(RS_gas); 
  Serial.print("Rs/R0 = "); 
  Serial.println(ratio); 
  Serial.print("\n\n"); 
 
  delay(100); 
 
}

// TERMOKOPEL SETUP DAN LOOP
void termo_setup() {


  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void termo_loop() {
  // basic readout test, just print the current temp
  
   Serial.print("C = "); 
   Serial.println(thermocouple.readCelsius());
   Serial.print("F = ");
   Serial.println(thermocouple.readFahrenheit());
 
   // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
   delay(1000);
}

// CO2 SETUP DAN LOOP

void CO2_setup(){
  CO2_sens.begin(v400, v40000);
  /*
   * Calibration mode
   * pinMode(CALIB_PIN, INPUT);
   * if(digitalRead(CALIB_PIN) == HIGH) {
   *  CO2_sens.calibrate();
   * }
   */
  
}

void CO2_loop(){
  //Print CO2 concentration
  Serial.print("CO2 Concentration: ");
  Serial.print(CO2_sens.read());
  Serial.println("ppm");
  
  delay(2000);
}
