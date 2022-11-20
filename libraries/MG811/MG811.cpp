/**
    Powered by Smart Technology Benin
    
    @autor: AMOUSSOU Z. Kenneth
    @date: 14-09-2018
    @version: 1.0
*/
#include "MG811.h"

/**
    Constructor
*/
MG811::MG811(float v_res, int adc_bit, int input, float ratio_clean_air, float ratio_dirty_air){
    _input = input;
    _adc_bit = pow(2, adc_bit) - 1;
    _v_res = v_res;
    v_clean_air = ratio_clean_air*v_res;
    v_dirty_air = ratio_dirty_air*v_res;
    _a = (ratio_clean_air*v_res - ratio_dirty_air*v_res)/(log10(400) - log10(40000)); // Delta V
}

/**
    function: init
    @summary: Initialize the usage of the sensor with calibrated value
    @parameter:
        v400: the voltage measured by the sensor when placed in a 400ppm C02 
              environment (outdoor or indoor with very good aeration
        v40000: the voltage measured by the sensor when placed in a 40000ppm C02 
              environment
    @return: none
*/
void MG811::init(int time_in_ms) {
    pinMode(_input, INPUT);
    sampling_time = time_in_ms;
}

/**
    function: raw
    @summary: measure multiple raw data from the sensor and compute the mean
    @parameter: none
    @return:
        float: return the voltage measured from the sensor
*/
float MG811::raw(bool bit){
    if(!bit) {
        float read, avg;
        int i;
        for (i = 0; i < 5; i++) {
            read += analogRead(_input);
            delay(sampling_time);
        }
        avg = read / i;
        return avg*_v_res/_adc_bit;
    }
    else return analogRead(_input);
}

/**
    function: read
    @summary: measure voltage from the sensor and compute the CO2 ppm value
    @parameter: none
    @return:
        float: return the CO2 concentration in 'ppm'
        
        Formulas:
            deltaV = (V400 - V40000)
            A = deltaV/(log10(400) - log10(40000))
            B = log10(400)
            C = (<measurement from sensor in volt> - V400)/A + B
            <CO2 ppm> = pow(10, C)
*/
float MG811::read(){
    static float C;
    C = (raw() - v_clean_air)/_a + log10(400);
    return pow(10, C);
}

/**
    function: calibrate
    @summary: calibrate the sensor to get reference value for measurement
              
              Power on the sensor
              
              [0] Put the sensor outdoor or indoor with good ventilation
                  Wait at least two (02) hours - for warming up
                  Read it's measurement - You get 400ppm reference voltage
              
              [1] Put the sensor in a bag filled with exhaled air
                  Wait a couple of minutes
                  Read it's measurement - You get 40000ppm reference voltage
              
              The reference value measured by this function should be used with 
              the `begin` method in order to use tthe sensor
              
    @see: this function needs Serial communication to be enabled to print out   
          information
          
          Serial.begin(9600)
    @parameter: none
    @return: none
*/
void MG811::calibrate(bool mode, int duration_in_s, int sampling_time_in_s){
    delay(3000);
    Serial.println("Entering MG811 calibration");
    Serial.printf("Mode: %s\n", mode ? "Clean Air" : "Dirty Air");
    Serial.println("Ratio");
    float read, avg;
    int i;
    for(i = 0; i < duration_in_s; i++){
        read += raw();
        delay(sampling_time_in_s*1000);
    }
    avg = read / i;
    Serial.printf("Average: %f\n", avg);
    if(mode) v_clean_air = avg;
    else v_dirty_air = avg;
}


