/*
 * SPI Worker for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author     Justin Berkshire
 * @copyright   2022
 * @licence     MIT
 *
 */
#include "main.h"
using namespace std;
//default pins and positions. 
uint stepper_one_coil_one[2] = { 2,3 };
uint stepper_one_coil_two[2] = { 4,5 };
uint stepper_one_f_pin = 10;
uint stepper_one_r_pin = 11;
uint stepper_two_coil_one[2] = {6, 7};
uint stepper_two_coil_two[2] = {8, 9};
uint stepper_two_f_pin = 12;
uint stepper_two_r_pin = 13;
uint additional_outputs[2] = {14, 15};
uint additional_inputs[3] = {20 ,21, 22};
const uint LED = 25;
//module startups and constructions to be shared between cores and scopes for global accessability
Peripherals periphers_state;
SpiInterface com(&periphers_state);
L293D_STEPPER stepper_one(stepper_one_coil_one, stepper_one_coil_two, 
periphers_state.stepper_one_gear, stepper_one_f_pin, stepper_one_r_pin);
L293D_STEPPER stepper_two(stepper_two_coil_one, stepper_two_coil_two, 
periphers_state.stepper_two_gear, stepper_two_f_pin, stepper_two_r_pin);
MotorControl motors(&stepper_one, &stepper_two, &periphers_state);
PCA9685 servo_controller(0.0f, 180.0f, 64, 0, 1);
AdcController adc;
ExtraGPIO simplePins(additional_inputs, additional_outputs);

void core1_movement_and_coms() {
    try {
        while(true) {
            int byteRead = com.exchangeByteMessage();
            if(byteRead == 1) {
                gpio_put(LED, periphers_state.L[0]);
                for(int i = 0; i < 16; i++) {
                    servo_controller.servoSetAngle(periphers_state.S[i], i);
                };
                stepper_one.setNewGearReduction(periphers_state.stepper_one_gear);
                stepper_two.setNewGearReduction(periphers_state.stepper_two_gear);
                motors.homeMotors();
                motors.checkAndActOnNewMoves();
                simplePins.setOutputState(periphers_state.extraOutPuts);
            }
        }
    } catch (...) {
        return;
    }
}

int main() {
    try {

        multicore_reset_core1();

        multicore_launch_core1(core1_movement_and_coms);

        gpio_init(LED);

        gpio_set_dir(LED, GPIO_OUT);

        gpio_put(LED, 1);

        while (true) {
            if(periphers_state.pause_updates == false) {
                periphers_state.updates_occurring = true;
                periphers_state.temperature = adc.getCoreTemperature();
                periphers_state.touch_one = adc.getInputvalue(0);
                periphers_state.touch_two = adc.getInputvalue(1);
                periphers_state.touch_three = adc.getInputvalue(2);
                periphers_state.touch_four = adc.getInputvalue(3);
                periphers_state.endstop_one = gpio_get(stepper_one_f_pin);
                periphers_state.endstop_two = gpio_get(stepper_one_r_pin);
                periphers_state.endstop_three = gpio_get(stepper_two_f_pin);
                periphers_state.endstop_four = gpio_get(stepper_two_r_pin);
                uint newInputValues[3];
                simplePins.getInputReads(newInputValues);
                for(int i = 0; i < 3; i++) {
                    periphers_state.extraInputs[i] = newInputValues[i];
                }
                periphers_state.updates_occurring = false;
            }
        }
        return 0;
    } catch (...) {
        return 1;
    }
    
}
