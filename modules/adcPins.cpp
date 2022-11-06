#include "../main.h"

class AdcController {
    public: AdcController() {
        adc_init();
        adc_set_temp_sensor_enabled(true);
    }
    public: float getCoreTemperature() {
        try {
            adc_select_input(4);

            uint16_t raw = adc_read();
            const float conversion = 3.3f / (1<<12);        
            float voltage = raw * conversion;
            float temperature = 27 - (voltage -0.706)/0.01721;      
            return temperature; 

        } catch (...) {
            // temperature will conceivably never be exactly zero, this will denote an error state
            return 0;
        }
    }
    public: float getInputvalue(uint adcToPole) {
        try {

            if(adcToPole > 3) throw;

            adc_select_input(adcToPole);

            return adc_read();

        } catch (...) {
            // if there's a failure this is to be treated the same as no input
            return 0;
        }
    }
};