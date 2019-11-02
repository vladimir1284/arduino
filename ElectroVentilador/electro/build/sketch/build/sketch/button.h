#include <Arduino.h>

// Time delay for detecting a long press
#define LONG_TIME 2000 // ms
// Time delay for debouncing the button
#define SHORT_TIME 200 // ms

class Button
{

public:
    Button();
    void run(); // Main method to be refreshed in every loop
    void init(int pin),
        clear();
    bool longPressed(),
        shortPressed();

private:
    int btnPin;
    unsigned int startActive;
    bool shortPressedDetected,
        longPressedDetected,
        active;
};