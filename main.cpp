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

int main() {

    stdio_init_all();

    uint stepper_one_coil_one[2] = { 2,3 };
    uint stepper_one_coil_two[2] = { 4,5 };
    uint stepper_one_f_pin = 10;
    uint stepper_one_r_pin = 11;

    uint stepper_two_coil_one[2] = {6, 7};
    uint stepper_two_coil_two[2] = {8, 9};
    uint stepper_two_f_pin = 12;
    uint stepper_two_r_pin = 13;

    const uint LED = 25;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    Peripherals periphers_state;

    SpiInterface com(&periphers_state);

    L293D_STEPPER stepper_one(stepper_one_coil_one, stepper_one_coil_two, 
    periphers_state.stepper_one_gear, stepper_one_f_pin, stepper_one_r_pin);

    L293D_STEPPER stepper_two(stepper_two_coil_one, stepper_two_coil_two, 
    periphers_state.stepper_two_gear, stepper_two_f_pin, stepper_two_r_pin);

    MotorControl motors(&stepper_one, &stepper_two, &periphers_state);

    PCA9685 servo_controller(0.0f, 180.0f, 64, 0, 1);

    AdcController adc;

    // string c = periphers_state.generate_new_response();

    while (true) {
        int byteRead = com.exchangeByteMessage();
        if(byteRead == 1) {
            gpio_put(LED, periphers_state.L[0]);
            servo_controller.servoSetAngle(periphers_state.S[0], 0);
            servo_controller.servoSetAngle(periphers_state.S[1], 1);
            servo_controller.servoSetAngle(periphers_state.S[2], 2);
            servo_controller.servoSetAngle(periphers_state.S[3], 3);
            servo_controller.servoSetAngle(periphers_state.S[4], 4);
            servo_controller.servoSetAngle(periphers_state.S[5], 5);
            servo_controller.servoSetAngle(periphers_state.S[6], 6);
            servo_controller.servoSetAngle(periphers_state.S[7], 7);
            servo_controller.servoSetAngle(periphers_state.S[8], 8);
            servo_controller.servoSetAngle(periphers_state.S[9], 9);
            servo_controller.servoSetAngle(periphers_state.S[10], 10);
            servo_controller.servoSetAngle(periphers_state.S[11], 11);
            servo_controller.servoSetAngle(periphers_state.S[12], 12);
            servo_controller.servoSetAngle(periphers_state.S[13], 13);
            servo_controller.servoSetAngle(periphers_state.S[14], 14);
            servo_controller.servoSetAngle(periphers_state.S[15], 15);
            stepper_one.setNewGearReduction(periphers_state.stepper_one_gear);
            stepper_two.setNewGearReduction(periphers_state.stepper_two_gear);
            motors.checkAndActOnNewMoves();
            motors.homeMotors();
        }
        periphers_state.temperature = adc.getCoreTemperature();
        periphers_state.touch_one = adc.getInputvalue(0);
        periphers_state.touch_two = adc.getInputvalue(1);
        periphers_state.touch_three = adc.getInputvalue(2);
        periphers_state.touch_four = adc.getInputvalue(3);
        periphers_state.endstop_one = gpio_get(stepper_one_f_pin);
        periphers_state.endstop_two = gpio_get(stepper_one_r_pin);
        periphers_state.endstop_three = gpio_get(stepper_two_f_pin);
        periphers_state.endstop_four = gpio_get(stepper_two_r_pin);
        
    }

    return 0;
}
