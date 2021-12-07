#include "gps.h"

// Constructor
GPS::GPS(HardwareSerial *softSerial)
{
    ss = softSerial;
}

//--------------------------------------------------------------------
void GPS::setup()
{
    ss->begin(GPSBaud);
}

//--------------------------------------------------------------------
int GPS::run()
{
    // Feed the GPS object
    while (ss->available()){
            gps.encode(ss->read());
    }
}

