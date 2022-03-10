/* 
 
  MQ9 
 
  modified on 19 Feb 2019 
  by Saeed Hosseini 
  https://electropeak.com/learn/ 
 
*/ 
 


 
void setup() { 
  Serial.begin(9600); 
 
} 
 
void loop() { 
 
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
 
  delay(1000); 
 
}
