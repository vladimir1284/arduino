#include <Arduino.h>

#define NCHECK 3
#define RMPCONST 15000

class RPM
{

public:
    RPM();

    void run(); // Main method to be refreshed in every loop
    void init(int pin);
    int getRPM();

private:
    int digitalPIN,
        edgeCount,
        innerCount;

    bool lastState,
        currentState;

    int startCount;
};
