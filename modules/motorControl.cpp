#include "../main.h"
using namespace std;

class MotorControl {
    private: L293D_STEPPER *stepper_1;
    private: L293D_STEPPER *stepper_2;
    private: Peripherals *periphers;
    public: MotorControl(L293D_STEPPER *motor_1, L293D_STEPPER *motor_2, Peripherals *p) {
        stepper_1 = motor_1;
        stepper_2 = motor_2;
        periphers = p;
    }
    public: void checkAndActOnNewMoves() {
        try {

            NewMove stepperOneMove = stepper_1->calculateNewMove(periphers->ST[0], 2, true);
            NewMove stepperTwoMove = stepper_2->calculateNewMove(periphers->ST[1], 2, true);

            while(stepperOneMove.steps > 0 && stepperTwoMove.steps > 0) {
                stepper_1->step(stepperOneMove.direction, 1, 0, stepperOneMove.hold);
                stepper_2->step(stepperTwoMove.direction, 1, 0, stepperTwoMove.hold);
                sleep_ms(periphers->stepper_speed);
                stepperOneMove.steps--;
                stepperTwoMove.steps--;
            }

        } catch (...) {
            printf("Failure to Check and act on new movements sent from controller;");
        }
    }
    public: void homeMotors() {
        stepper_1->home_joint_to_endstop();
        stepper_2->home_joint_to_endstop();
    }
};