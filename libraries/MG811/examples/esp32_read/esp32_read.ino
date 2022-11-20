#include <MG811.h>

#define V_RES 3.3 // ESP32 voltage input is 3.3
#define ADC_BIT 12 // ESP32 ADC BIT is 12
#define PIN 37 // Use GPIO37

MG811 CO2sens(V_RES, ADC_BIT, PIN);

void setup(){
    Serial.begin(9600);
    
    CO2sens.init();
}


void loop(){
    Serial.printf("Raw voltage: %f\n", CO2sens.raw()); // getting raw data
    Serial.printf("Concentration: %fppm\n", CO2.read()); // getting ppm data
    delay(1000);
}
