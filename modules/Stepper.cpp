#include "../main.h"
using namespace std;

class L293D_STEPPER {
    private: int reductionFactor;
    private: uint coilOne[2];
    private: uint coilTwo[2];
    private: float currentAngle = 0.0f;
    private: float stepAngleMultiplier = 1.8;
    private: float stepAngle;
    private: uint reduction[2];
    private: uint f_home_pin;
    private: uint r_home_pin;
    private: int max_homing_rotations = 500;
    public: bool homed = false;
           //Nema 17 provides 1.8 degrees per step so that'll be the standard, this can be set on initialization;
    public: L293D_STEPPER(uint firstCoil[2], uint secondCoil[2], uint gearReduction[2], uint fPin, uint rPin, float motorAnglePerStep = 1.8) {
        //initialize coil one pins;
        for (int i = 0;i < 2;i++) {
            gpio_init(firstCoil[i]);
            gpio_set_dir(firstCoil[i], GPIO_OUT);
            gpio_put(firstCoil[i], 0);
            coilOne[i] = firstCoil[i];
        }
        //initialize coil two pins
        for (int i = 0;i < 2;i++) {
            gpio_init(secondCoil[i]);
            gpio_set_dir(secondCoil[i], GPIO_OUT);
            gpio_put(secondCoil[i], 0);
            coilTwo[i] = secondCoil[i];
        }
        //initialize endstop pins
        f_home_pin = fPin;
        r_home_pin = rPin;
        gpio_init(f_home_pin);
        gpio_set_dir(f_home_pin, GPIO_IN);
        gpio_pull_up(f_home_pin);
        gpio_init(r_home_pin);
        gpio_set_dir(r_home_pin, GPIO_IN);
        gpio_pull_up(r_home_pin);

        reduction[0] = gearReduction[0];
        reduction[1] = gearReduction[1];
    
        stepAngle = motorAnglePerStep;
    
        reductionFactor = gearReduction[1] / gearReduction[0];
    
        stepAngleMultiplier = motorAnglePerStep / (gearReduction[1] / gearReduction[0]);
    
        releaseMotor();
    }
    public: float getCurrentAngle() {
        return currentAngle;
    }
    public: float setCurrentAngle(float angle) {
        currentAngle = angle;
        return currentAngle;
    }
    public: void setNewGearReduction(uint ngr[2]) {
    
        reduction[0] = ngr[0];
        reduction[1] = ngr[1];
    
        reductionFactor = ngr[1] / ngr[0];
    
        stepAngleMultiplier = stepAngle / (ngr[1] / ngr[0]);
    
    }
    public: void moveToNewAngle(float newAngle, int speed = 2, bool hold = true) {
        float angleDifference;
        char newDirection;
        int steps;

        if(newAngle == currentAngle) return;

        if (newAngle > currentAngle) {
            newDirection = 'f';
            angleDifference = newAngle - currentAngle;
        }
        else {
            newDirection = 'r';
            angleDifference = currentAngle - newAngle;
        }

        steps = angleDifference / stepAngleMultiplier;

        currentAngle = newAngle;

        step(newDirection, steps, speed, hold);

    }
    public: NewMove calculateNewMove(float newAngle, int speed, bool hold = true) {
        float angleDifference;
        char newDirection;
        int steps;

        if(newAngle == currentAngle) {
            NewMove noOp('r', 0, 2, true);
            return noOp;
        };
        if (newAngle > currentAngle) {
            newDirection = 'f';
            angleDifference = newAngle - currentAngle;
        }
        else {
            newDirection = 'r';
            angleDifference = currentAngle - newAngle;
        }

        steps = angleDifference / stepAngleMultiplier;

        currentAngle = newAngle;

        NewMove nm(newDirection, steps, speed, hold);

        return nm;

    }
    private: void setCoilDirection(char direction, uint coil[2]) {
        // if (direction != 'r' && direction != 'f') return;//invalid direction may throw in future;
        // if (coil != coilOne && coil != coilTwo) return; // invalid coil set throw event;

        uint coilSequence[2];

        if (direction == 'f') {
            coilSequence[0] = coil[0];
            coilSequence[1] = coil[1];
        }
        if (direction == 'r') {
            coilSequence[0] = coil[1];
            coilSequence[1] = coil[0];
        }

        gpio_put(coilSequence[0], 1);
        gpio_put(coilSequence[1], 0);

    }
    public: void releaseMotor() {
        for (int i = 0;i < 2;i++) {
            gpio_put(coilOne[i], 0);
        }
        for (int i = 0;i < 2;i++) {
            gpio_put(coilTwo[i], 0);
        }
    }
    public: void step(char direction, int steps, int speed = 2, bool hold = true) {
        if (speed < 2) return; //can't move shaft faster than 60rpm
        if (direction != 'r' && direction != 'f') return;// direction can only be r or f
        char polarities[2];
        uint coils[2][2];

        if (direction == 'f') {
            polarities[0] = 'f';
            polarities[1] = 'r';
            coils[0][0] = coilOne[0];
            coils[0][1] = coilOne[1];
            coils[1][0] = coilTwo[0];
            coils[1][1] = coilTwo[1];
        }
        if (direction == 'r') {
            polarities[0] = 'r';
            polarities[1] = 'f';
            coils[0][0] = coilTwo[0];
            coils[0][1] = coilTwo[1];
            coils[1][0] = coilOne[0];
            coils[1][1] = coilOne[1];
        }

        char home_pin_pressed;

        while (steps > 0) {
            for (int d = 0; d < 2;d++) {
                for (int c = 0; c < 2; c++) {
                    setCoilDirection(polarities[d], coils[c]);
                    home_pin_pressed = check_home_pins();
                    if(home_pin_pressed == 'f' && direction == 'f') steps = 0;
                    if(home_pin_pressed == 'r' && direction == 'r') {
                        currentAngle = 0.f;
                        steps = 0;
                    }
                    sleep_ms(speed);
                    steps--;
                }
            }
        }

        if (!hold) releaseMotor();
    }
    public: void home_joint_to_endstop() {
        //initialize r side pin press var to track throughout looping
        char endstop_pin_press = 0;
        //move stepper in forward direction to make sure it's not resting on r end stop or any endstop;
        step('f', 50, 2);
        //move in r direction until an endstop is reached
        while(endstop_pin_press == 0) {
            step('r', 5, 10);
            endstop_pin_press = check_home_pins();
        }
        //if non r endstop is the one reached we need to reverse the pin assignments to finish homing
        if(endstop_pin_press != 'r') swap_homing_pins();
        sleep_ms(200);
        // move forward again so we're not resting on an endstop after a breif rest
        step('f', 30, 2);
        // if there has been a sleep the above step motion will not occur at all and be skipped entirely
        endstop_pin_press = 0; // reset endstop press for final homing motion. 
        int rotations = 0; // failsafe in case of critical failure, it'll stop trying to home after 500 iterations of 3 count step rotations. this can be set higher via spi command;
        while(endstop_pin_press != 'r' || rotations < max_homing_rotations) {
            step('r', 3, 20);
            endstop_pin_press = check_home_pins();
            rotations++;
        }

        if(endstop_pin_press == 'r' && currentAngle == 0.f) homed = true;
    }
    private: char check_home_pins() {
        uint f_pin_state = gpio_get(f_home_pin);
        if(f_pin_state > 0) return 'f';
        uint r_pin_state = gpio_get(r_home_pin);
        if(r_pin_state > 0) return 'r';
        return 0;
    }
    private: void swap_homing_pins() {
        uint hold_pin = f_home_pin;
        f_home_pin = r_home_pin;
        r_home_pin = hold_pin;
    }
};
