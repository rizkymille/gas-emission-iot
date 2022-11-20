/**
    Powered by Smart Technology Benin
    
    @autor: AMOUSSOU Z. Kenneth
    @date: 14-09-2018
    @version: 1.0
*/
#ifndef H_MG811
#define H_MG811

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#define DEBUG

class MG811{
    public:
        MG811(float v_res = 5, int adc_bit = 8, int input = 1, float ratio_clean_air = 0.3, float ratio_dirty_air = 0.9);
        void init(int time_in_ms = 50);
        float raw(bool bit = false);         // return raw data (in volt) from the sensor
        void calibrate(bool mode = false, int duration_in_s = 10, int sampling_time_in_s = 1);   // set up the sensor by calibrating two references
        float read();       // return the air quality in 'ppm'
        
    private:
        float _v_res;
        float _a;
        int _adc_bit, _input;
        float v_clean_air, v_dirty_air;
        int sampling_time;
};

#endif
