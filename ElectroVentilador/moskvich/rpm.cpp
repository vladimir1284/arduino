#include "rpm.h"

// Constructor
RPM::RPM()
{
}

//-----------------------------------------------------
void RPM::init(int pin)
{
    digitalPIN = pin;
    pinMode(digitalPIN, INPUT);

    lastState = digitalRead(digitalPIN);

    startCount = millis();
}

//-----------------------------------------------------
int RPM::getRPM()
{
    int elapsedTime = millis() - startCount;
    int rpm = edgeCount * RMPCONST / elapsedTime;
    edgeCount = 0;
    startCount = millis();
    return rpm;
}

//-----------------------------------------------------
void RPM::run()
{
    currentState = digitalRead(digitalPIN);

    if (currentState != lastState)
    {
        innerCount++;
    }
    else
    {
        innerCount = 0;
    }

    if (innerCount == NCHECK)
    {
        edgeCount++;
        innerCount = 0;
        lastState = currentState;
    }
}