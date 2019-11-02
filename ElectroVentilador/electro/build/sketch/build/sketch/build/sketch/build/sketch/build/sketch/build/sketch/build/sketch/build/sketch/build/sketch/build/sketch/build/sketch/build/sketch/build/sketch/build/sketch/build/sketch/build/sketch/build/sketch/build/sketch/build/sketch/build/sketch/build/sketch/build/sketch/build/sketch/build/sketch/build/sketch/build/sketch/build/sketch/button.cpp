#include "button.h"

//  Constructor
Button::Button()
{
    active = false;
    clear();
}

void Button::clear()
{
    longPressedDetected = false;
    shortPressedDetected = false;
}

//  Initial setup
void Button::init(int pin)
{
    btnPin = pin;
    pinMode(btnPin, INPUT_PULLUP);
}

//  loop
void Button::run()
{
    int val = digitalRead(btnPin);
    if (active)
    {
        if (val == HIGH)
        {
            active = false;
            // If the required holding time for a long press have been reached
            if (millis() - startActive > LONG_TIME)
            {
                longPressedDetected = true;
                shortPressedDetected = false;
            }
            else
            {
                // If the required debounce time for a short press have been reached
                if (millis() - startActive > SHORT_TIME)
                {
                    shortPressedDetected = true;
                }
            }
        }
    }
    else
    {
        if (val == LOW)
        {
            active = true;
            startActive = millis();
        }
    }
}

//  Return true if a long pressed was detected
bool Button::longPressed()
{
    bool result = longPressedDetected;
    if (longPressedDetected)
    {
        longPressedDetected = false;
    }
    return result;
}

//  Return true if a short pressed was detected
bool Button::shortPressed()
{
    bool result = shortPressedDetected;
    if (shortPressedDetected)
    {
        shortPressedDetected = false;
    }
    return result;
}