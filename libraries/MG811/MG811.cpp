#include "MG811.h"


MG811::MG811(const float& v_res, const int& adc_bit, const int& input, const float& ratio_clean_air, const float& ratio_dirty_air){
    _input = input;
    _adc_bit = pow(2, adc_bit) - 1;
    _v_res = v_res;
    v_clean_air = ratio_clean_air*v_res;
    v_dirty_air = ratio_dirty_air*v_res;
    _a = (ratio_clean_air*v_res - ratio_dirty_air*v_res)/(log10(ppm_clean_air) - log10(ppm_dirty_air));
}

void MG811::set_clean_air(const float& ratio_clean_air, const float& ppm) {
    ppm_clean_air = ppm;
    v_clean_air = ratio_clean_air*_v_res;
}

void MG811::set_dirty_air(const float& ratio_dirty_air, const float& ppm) {
    ppm_dirty_air = ppm;
    v_dirty_air = ratio_dirty_air*_v_res;
}

void MG811::init(const int& time_in_ms) {
    pinMode(_input, INPUT);
    sampling_time = time_in_ms;
}

float MG811::raw(const bool& bit){
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

float MG811::read(){
    static float C;
    C = (raw() - v_clean_air)/_a + log10(ppm_clean_air);
    return pow(10, C);
}

void MG811::calibrate(const bool& mode, const int& duration_in_s, const int& sampling_time_in_s){
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


