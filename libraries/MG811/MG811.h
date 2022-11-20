#ifndef MG811_H_
#define MG811_H_

#include <Arduino.h>

class MG811{
    public:
        MG811(const float& v_res = 5, const int& adc_bit = 8, const int& input = 1, const float& ratio_clean_air = 0.6412, const float& ratio_dirty_air = 0.907);
        void init(const int& time_in_ms = 50);
        void set_clean_air(const float& ratio_clean_air, const float& ppm);
        void set_dirty_air(const float& ratio_dirty_air, const float& ppm);
        float raw(const bool& bit = false);         // return raw data (in volt) from the sensor
        void calibrate(const bool& mode = false, const int& duration_in_s = 10, const int& sampling_time_in_s = 1);   // set up the sensor by calibrating two references
        float read();       // return the air quality in 'ppm'
        
    private:
        float _v_res;
        float _a;
        int _adc_bit, _input;
        float v_clean_air, v_dirty_air;
        float ppm_clean_air = 400;
        float ppm_dirty_air = 40000;
        int sampling_time;
};

#endif // MG811_H_
