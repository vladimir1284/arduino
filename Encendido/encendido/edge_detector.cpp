#include "edge_detector.h"

// Constructor
EdgeDetector::EdgeDetector(int uPin)
{
    pin = uPin;
    state = LOW;
    count = 0;
}

//--------------------------------------------------------------------
// Function for detecting a falling edge rejecting noisy readings
bool EdgeDetector::detect_falling()
{
    detect_edge(LOW);
}

//--------------------------------------------------------------------
// Function for detecting a rising edge rejecting noisy readings
bool EdgeDetector::detect_rising()
{
    detect_edge(HIGH);
}

//--------------------------------------------------------------------
// Function for detecting an edge rejecting noisy readings
bool EdgeDetector::detect_edge(bool rising)
{
    bool value = digitalRead(pin);
    if (state == !rising)
    {
        if (value == rising) // detected for the first time
        {
            count = 1;
        }
    }
    else
    {
        if (count > 0)
        { // verify several times for noise reject
            if (value == rising)
            {
                count++;
            }
            else
            { // reset if a previous value is detected
                count = 0;
            }
            if (count == NSAMPLES)
            { // acknowledge an edge
                count = 0;
                return true;
            }
        }
    }
    state = value;
    return false;
}