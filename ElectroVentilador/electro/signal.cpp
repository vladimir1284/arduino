#include "signal.h"

// Constructor
ExternalSignal::ExternalSignal()
{
}

//-----------------------------------------------------
void ExternalSignal::init(int pin)
{
    digitalPIN = pin;
    pinMode(digitalPIN, INPUT_PULLDOWN);

    estado = INACTIVE;
}

//-----------------------------------------------------
bool ExternalSignal::isActive()
{
    return (estado == ACTIVE);
}

//-----------------------------------------------------
void ExternalSignal::run()
{
    switch (estado)
    {
    case INACTIVE:
        if (digitalRead(digitalPIN) == HIGH)
        {
            estado = WAIT;
            startWait = millis();
        }
        break;
    case WAIT:
        if (millis() - startWait > WAIT2ACTIVATE)
        {
            estado = ACTIVE;
        }
        if (digitalRead(digitalPIN) == LOW)
        {
            estado = INACTIVE;
        }
        break;
    case ACTIVE:
        if (digitalRead(digitalPIN) == LOW)
        {
            estado = INACTIVE;
        }
        break;
    default:
        break;
    }
}