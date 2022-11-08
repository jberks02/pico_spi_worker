#include "../main.h"

class Command {
    public: int length;
    public: uint16_t body[0] = {};
    public: Command(string c) {

    }
};
