#include <max6675.h>
#include <MQUnifiedsensor.h>
#include <MG811.h>

#include <SPI.h>

// CO2 sensor params
#define CO2_PIN 33 // GPIO ESP32
#define CALIB_PIN 4 // GPIO ESP32

#define v400 4.535
#define v40000 3.206

// Thermocouple sensor params
#define MAX6675_CS 10
#define MAX6675_MISO 12
#define MAX6675_SCLK 13

// CO sensor params
#define BOARD "ESP32"
#define CO_SENS_PIN 25

#define TYPE "MQ-9"
#define VOLT_RESO 5
#define ADC_BIT_RESO 12
#define RATIO_MQ9_CLEAN 9.6

MG811 CO2_sens(CO2_PIN);
MQUnifiedsensor CO_sens(BOARD, VOLT_RESO, ADC_BIT_RESO, CO_SENS_PIN, TYPE);
MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);

int calib_counter;

void MQ9_calibration() {
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    CO_sens.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += CO_sens.calibrate(RATIO_MQ9_CLEAN);
    Serial.print(".");
    delay(10000);
  }
  CO_sens.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}
}

void setup() {
  Serial.begin(57600);

  CO2_sens.begin(v400, v40000);
  pinMode(CALIB_PIN, INPUT);

  // Enter calibration mode
  while(digitalRead(CALIB_PIN) == HIGH) {
    calib_counter++;
    delay(1000);
  }

  if(calib_counter == 3) {
    CO2_sens.calibrate();
  }

  CO_sens.setRegressionMethod(1);
  CO_sens.setA(1000.5); 
  CO_sens.setB(-2.186);
  CO_sens.init();
  MQ9_calibration();
}

void loop() {
  // Get CO2 data
  Serial.print("CO2 Concentration: ");
  Serial.print(CO2_sens.read());
  Serial.println("ppm");

  // Get CO data
  MQ9.update();
  Serial.print("CO Concentration: ");
  Serial.print(MQ9.readSensor());
  Serial.println("ppm");

  // Get temperature data
  Serial.print("Temperature:");
  Serial.print(thermocouple.readCelsius());
  Serial.println("°C")

  delay(2000);
}