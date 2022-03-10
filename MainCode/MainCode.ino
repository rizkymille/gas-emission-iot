#include "Function.h"

//sensor :
//lora s913 
//anemometer v
//suhu termokopel v 
//humidity sensor tbc
//co2 sensor MQ11 V 
//co sensor MQ7 V

void setup() {
 CO_setup();
 termo_setup();
 CO2_setup();


}

void loop() {
 CO_loop();
 termo_loop();
 CO2_loop();


}
