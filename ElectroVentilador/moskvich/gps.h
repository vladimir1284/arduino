#if !defined(GPS_H)
#define GPS_H

#include "configs.h"
#include <TinyGPS++.h>

class GPS
{

public:
    GPS(HardwareSerial *softSerial);

    void setup();

    int run();

    TinyGPSPlus gps;

private:
    // The TinyGPS++ object
    HardwareSerial *ss;
};

#endif // GPS_H