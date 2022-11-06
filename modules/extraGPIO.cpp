#include "../main.h"

class ExtraGPIO {
    int inputLength = 3;
    uint inputList[3];
    uint outputList[2];
    int outputSize = 2;
    public: ExtraGPIO(uint inputs[3], uint outputs[2]) {
        for (int i = 0; i < inputLength; i++) {
            gpio_init(inputs[i]);
            gpio_set_dir(inputs[i], GPIO_IN);
            gpio_pull_up(inputs[i]);
            inputList[i] = inputs[i];
        }
        for(int i = 0; i < outputSize;i++) {
            gpio_init(outputs[i]);
            gpio_set_dir(outputs[i], GPIO_OUT);
            gpio_pull_down(outputs[i]);
            gpio_put(outputs[i], 0);
            outputList[i] = outputs[i];
        }
    }
    public: uint getInputReads(uint newState[3]) {

        for(int i = 0; i < inputLength; i++) {
            newState[i] = gpio_get(inputList[i]);
        }

        return *newState;
    }
    public: void setOutputState(uint puts[2]) {
        for(int i = 0; i < 2; i++) {
            gpio_put(outputList[i], puts[i]);
        }
    }
};
