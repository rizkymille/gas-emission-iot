#include "max6675.h"
#include "MQUnifiedsensor.h"
#include "MG811.h"
#include "lorawan.h"

#define CALIB_PIN -1 // GPIO ESP32

// CO2 sensor params
#define CO2_SENS_PIN 37 // GPIO ESP32

// Thermocouple sensor params
#define MAX6675_CS 27
#define MAX6675_MISO 12
#define MAX6675_SCLK 14
// use software SPI, the hardware SPI is already allocated for LoRa

// CO sensor params
#define BOARD "ESP32"
#define CO_SENS_PIN 39

#define TYPE "MQ-9"
#define V_RES 3.3
#define ADC_BIT 12
#define RATIO_MQ9_CLEAN 9.6
#define MQ9_R0 2.1

// LoRa params
// pins
#define RFM_ENABLE 32
#define RFM_CS 2
#define RFM_RST 13
#define RFM_DIO0 14
#define RFM_DIO1 12

#define RFM_FPORT 1
#define RFM_CHAN 0 // channel range 0-7 

//ABP Credentials
const char *devAddr = "260D5728";
const char *nwkSKey = "3AB71715B5225CA9654D838BBF61FB2B";
const char *appSKey = "307F9A1909726A248F0FC6B2EB0DED38";

//const unsigned long interval = 10000;    // 10 s interval to send message
//unsigned long previousMillis = 0;  // will store last time message sent

int port, channel, freq;


const sRFM_pins RFM_pins = {
  .CS = RFM_CS,
  .RST = RFM_RST,
  .DIO0 = RFM_DIO0,
  .DIO1 = RFM_DIO1,
};
//---------------------------------------
MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);
MG811 CO2_sens(V_RES, ADC_BIT, CO2_SENS_PIN);
MQUnifiedsensor CO_sens(BOARD, V_RES, ADC_BIT, CO_SENS_PIN, TYPE);

int calib_counter;

void MQ9_calibration() {
  Serial.print("MQ9 calibration started");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++) {
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
  Serial.begin(115200);

  digitalWrite(RFM_ENABLE, HIGH);

  /*
  pinMode(CALIB_PIN, INPUT);

  // Enter calibration mode
  while(digitalRead(CALIB_PIN) == HIGH) {
    calib_counter++;
    delay(1000);
  }
  */

  CO_sens.setRegressionMethod(1);
  CO_sens.setA(1000.5); 
  CO_sens.setB(-2.186);
  CO_sens.setR0(MQ9_R0);
  CO_sens.init();

  CO2_sens.init();

  /*
  if(calib_counter == 3) {
    CO2_sens.calibrate();
    MQ9_calibration();
  }
  */

  while(!lora.init()) {
    Serial.println("RFM95 not detected!");
    delay(2000);
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF10BW125);

  // Set FramePort Tx
  lora.setFramePortTx(RFM_FPORT); // set according to fport, must not 0

  // set channel to channel 0
  lora.setChannel(RFM_CHAN);
  //lora.setChannel(MULTI); // set channel to random

  // Set TxPower to 15 dBi (max)
  lora.setTxPower(15);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);

  delay(500); // wait sensors to stabilize
}

//char sendData[];

void loop() {
  static float CO2_val, CO_val, temp_val;
  static int status;

  // Get CO2 data
  CO2_val = CO2_sens.read();
  
  // Get CO data
  CO_sens.update();
  CO_val = CO_sens.readSensor();
  
  // Get temperature data
  temp_val = thermocouple.readCelsius();
  

  // Pack data
  //sprintf(sendData, "%f/%f/%f", CO2_val, CO_val, temp_val);
  
  // Send with LoRa
  //lora.sendUplink(sendData, strlen(sendData), 1);

  // print CO2 into serial
  Serial.print("CO2 Concentration: ");
  Serial.print(CO2_val);
  Serial.println("ppm");

  // print CO into serial
  Serial.print("CO Concentration: ");
  Serial.print(CO_val);
  Serial.println("ppm");

  // print temperature into serial
  Serial.print("Temperature: ");
  Serial.print(status);
  Serial.println("°C");

  /*
  // print sent data into serial
  Serial.println(sendData);
  
  // print fport, channel and freq into serial
  port = lora.getFramePortTx();
  channel = lora.getChannel();
  freq = lora.getChannelFreq(channel);
  Serial.print(F("fport: "));    Serial.print(port);Serial.print(" ");
  Serial.print(F("Ch: "));    Serial.print(channel);Serial.print(" ");
  Serial.print(F("Freq: "));    Serial.print(freq);Serial.println(" ");

  dijadiin comment karena kayaknya ada masalah antara serial port ESP32 Aurora
  ke PC sama port SPI. Tapi belum dites. 

  lora.update();
  */

  delay(1000);
}
