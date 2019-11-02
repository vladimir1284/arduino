#include <Arduino.h>
#include "median_filter.h"

#define MAX_SENSORS 2 // Maximun number of SR04 sensor connected


void setupUltrasonicSensors(int trigerPin, int nSensors, int echoPins[]),
    handler_trigger_on(),
    handler_trigger_off(),
    readSR04sensors(),
    exti_handler();

int getLevel(int sensor);
