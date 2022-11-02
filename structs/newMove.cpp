#include "../main.h"
using namespace std;

class NewMove {
    public: char direction;
    public: int steps;
    public: int speed;
    public: bool hold;
    public: NewMove(char d, int st, int sp, bool h) {
        direction = d;
        steps = st;
        speed = sp;
        hold = h;
    }
};