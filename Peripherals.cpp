#include <stdio.h>
#include <string.h>
#include "main.h"
using namespace std;

class Peripherals {
    public: uint L[1] = {0};
    public: uint S[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    public: uint ST[3] = {0,0,0};
    public: int process_command(uint8_t *command, int commandLength) {
        try {
            if(command[0] == 'L') {
                int index;
                string indexString = "";
                int lastIteration;

                for(int i = 1; i < commandLength; i++) {
                    lastIteration = i;
                    if(command[i] == ' ' && indexString == "") continue;
                    if(command[i] == ' ' && indexString != "") break;
                    indexString = indexString + (char)command[i];
                }

                index = atoi(indexString.c_str());

                string valueString = "";
                float value;

                for(int i = lastIteration; i < commandLength; i++) {
                    if(command[i] == ';') break;
                    valueString = valueString + (char)command[i];   
                }
                value = atoi(valueString.c_str());

                L[index] = value;
                return 0;
            } else {
                return -1;
            }
        } catch (...) {
            return -1;
        }
    }
};
