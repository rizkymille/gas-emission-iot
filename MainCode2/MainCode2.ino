#include <max6675.h>
#include <MQUnifiedsensor.h>
#include <MG811.h>

#include <lorawan.h>

#include <SPI.h>

#define CALIB_PIN -1 // GPIO ESP32

// CO2 sensor params
#define CO2_SENS_PIN 33 // GPIO ESP32

#define v400 4.535
#define v40000 3.206

// Thermocouple sensor params
#define MAX6675_CS 15
#define MAX6675_MISO 0
#define MAX6675_SCLK 5

// CO sensor params
#define BOARD "Arduino"
#define CO_SENS_PIN 35

#define TYPE "MQ-9"
#define VOLT_RESO 3.3
#define ADC_BIT_RESO 12
#define RATIO_MQ9_CLEAN 9.6
#define MQ9_R0 2.1

// LoRa params
// pins
#define RFM_ENABLE 32
#define RFM_CS 2
#define RFM_RST 13
#define RFM_DIO0 14
#define RFM_DIO1 12

#define RFM_FPORT 0
#define RFM_CHAN 0 // channel range 0-7 

//ABP Credentials
const char *devAddr = "018229BB";
const char *nwkSKey = "2541B4A7145935927393358617651B9B";
const char *appSKey = "BFF2CA2C7191F895365CCF822C3224D1";

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

MG811 CO2_sens(CO2_SENS_PIN);
MQUnifiedsensor CO_sens(BOARD, VOLT_RESO, ADC_BIT_RESO, CO_SENS_PIN, TYPE);
MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);

int calib_counter;

void MQ9_calibration() {
  Serial.print("MQ9 calibration started");
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
  Serial.begin(9600);

  pinMode(RFM_ENABLE, HIGH);

  CO2_sens.begin(v400, v40000);
  pinMode(CALIB_PIN, INPUT);

  // Enter calibration mode
  while(digitalRead(CALIB_PIN) == HIGH) {
    calib_counter++;
    delay(1000);
  }

  CO_sens.setRegressionMethod(1);
  CO_sens.setA(1000.5); 
  CO_sens.setB(-2.186);
  CO_sens.setR0(MQ9_R0);
  CO_sens.init();

  if(calib_counter == 3) {
    CO2_sens.calibrate();
    MQ9_calibration();
  }

  while(!lora.init()) {
    Serial.println("RFM95 not detected!");
    delay(2000);
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF10BW125);

  // Set FramePort Tx
  lora.setFramePortTx(RFM_FPORT); // set according to fport

  // set channel to channel 0
  lora.setChannel(RFM_CHAN);
  //lora.setChannel(MULTI); // set channel to random

  // Set TxPower to 15 dBi (max)
  lora.setTxPower(15);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

float CO2_val, CO_val, temp_val;
//char myStr[35];
byte sendData[3] = {1, 2, 3};

void loop() {
  // Get CO2 data
  CO2_val = CO2_sens.read();
  Serial.print("CO2 Concentration: ");
  Serial.print(CO2_val);
  Serial.println("ppm");

  // Get CO data
  //CO_val = analogRead(CO_SENS_PIN);
  CO_sens.update();
  CO_val = CO_sens.readSensor();
  Serial.print("CO Concentration: ");
  Serial.print(CO_val);
  Serial.println("ppm");

  // Get temperature data
  temp_val = thermocouple.readCelsius();
  Serial.print("Temperature: ");
  Serial.print(temp_val);
  Serial.println("°C");

  // Send with LoRa
  //sprintf(myStr, "CO2:%.2fppmCO:%.2fppmTemp:%.2f°C", CO2_val, CO_val, temp_val);
  //sprintf(myStr, "hello world");
  //sprintf(myStr, "%.2f", int(CO2_val);
  
  //Serial.print("Sending: \n");
  //Serial.println(myStr);
  lora.sendUplinkHex(sendData, sizeof(sendData), 0);
  //lora.sendUplink(myStr, strlen(myStr), 0);
  port = lora.getFramePortTx();
  channel = lora.getChannel();
  freq = lora.getChannelFreq(channel);
  Serial.print(F("fport: "));    Serial.print(port);Serial.print(" ");
  Serial.print(F("Ch: "));    Serial.print(channel);Serial.print(" ");
  Serial.print(F("Freq: "));    Serial.print(freq);Serial.println(" ");

  lora.update();
  delay(1000);
}
