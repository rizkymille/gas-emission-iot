#include <MQUnifiedsensor.h>

#define BOARD "ESP32"
#define CO_SENS_PIN 25

#define TYPE "MQ-9"
#define VOLT_RESO 5
#define ADC_BIT_RESO 12
#define RATIO_MQ9_CLEAN 9.6

MQUnifiedsensor MQ9(BOARD, VOLT_RESO, ADC_BIT_RESO, CO_SENS_PIN, TYPE);

void MQ9calibration() {
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ9.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ9.calibrate(RATIO_MQ9_CLEAN);
    Serial.print(".");
    delay(10000);
  }
  MQ9.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
}

void setup() { 
  MQ9.setRegressionMethod(1);
  MQ9.setA(1000.5); MQ9.setB(-2.186);
  MQ9.init();
  
  Serial.begin(57600); 

  MQ9calibration();
} 

void loop() { 
  MQ9.update();
  Serial.print("Concentration: ");
  Serial.println(MQ9.readSensor());
  delay(1000);
}
