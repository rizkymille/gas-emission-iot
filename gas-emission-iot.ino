#include "MQUnifiedsensor.h"
#include "MG811.h"
#include "lorawan.h"
#include "SerialTransfer.h"

#define CALIB_PIN -1 // GPIO ESP32

// CO2 sensor params
#define CO2_SENS_PIN 37 // GPIO ESP32

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

const sRFM_pins RFM_pins = {
  .CS = RFM_CS,
  .RST = RFM_RST,
  .DIO0 = RFM_DIO0,
  .DIO1 = RFM_DIO1,
};
//---------------------------------------

MG811 CO2_sens(V_RES, ADC_BIT, CO2_SENS_PIN);
MQUnifiedsensor CO_sens(BOARD, V_RES, ADC_BIT, CO_SENS_PIN, TYPE);
SerialTransfer serialParser;

void sendLora() {
  static float CO2_val, CO_val, temp_val;
  static int status;

  // Get CO2 data
  CO2_val = CO2_sens.read();
  
  // Get CO data
  CO_sens.update();
  CO_val = CO_sens.readSensor();
  
  // Get temperature data
  temp_val = getTemperatureData();
  
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
  Serial.print(F("fport: "));    Serial.print(lora.getFramePortTx());Serial.print(" ");
  Serial.print(F("Ch: "));    Serial.print(lora.getChannel());Serial.print(" ");
  Serial.print(F("Freq: "));    Serial.print(lora.getChannelFreq(channel));Serial.println(" ");

  dijadiin comment karena kayaknya ada masalah antara serial port ESP32 Aurora
  ke PC sama port SPI. Tapi belum dites. 

  lora.update();
  */
}

float getTemperatureData() {
  static uint16_t sendSize, recSize;
  static float temp;
  // change to interrupt mode later
  sendSize = 0;
  sendSize = serialParser.txObj("G", sendSize);
  serialParser.sendData(sendSize);

  while (serialParser.available()) {
    recSize = 0;
    recSize = serialParser.rxObj(temp, recSize);
  }

  sendSize = 0;
  sendSize = serialParser.txObj("R", sendSize);
  serialParser.sendData(sendSize);

  return temp;
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  serialParser.begin(Serial1);

  digitalWrite(RFM_ENABLE, HIGH);

  CO_sens.setRegressionMethod(1);
  CO_sens.setA(1000.5); 
  CO_sens.setB(-2.186);
  CO_sens.setR0(MQ9_R0);
  CO_sens.init();

  CO2_sens.init();

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

void loop() {}
