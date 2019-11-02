/**************************************************************************
  This is a library for controlling a domestic pumping system.
     ___                                 ___
  __|   |________________               |___| SR04 ultrasonic
 |          ____<<_____ ^|               ° °  sensor                   ___
 |  Upper  |           |^|                |                             ^
 |  Tank   |           | |                |--> gap (distance between    H
 |~~~~~~~~~| level    ( o ) Pump          |  sensor and maximun level)  e
 |_________|           | |                |                             i
                       |^|                |--> restart (% of the level  g
                level ||^|~~~~~~~~~~|     |        to restart pumping)  h
                      || |  Lower   |     |                             t
                      || |  Tank    |     |--> min (% of the level      
                level ||^|~~~~~~~~~~|     |     indicating empty tank)  |
                      |_____________|     |                            _|_

 **************************************************************************/
#include "TankParameters.h"
#include "SR04.h"

// Ultrasonic sensors configs
#define triggerPIN PB15
#define N_SENSORS 2
int ECHO_PINS[N_SENSORS] = {PA8, PA9}; // Echo pins
#define UPPERTANK 0
#define LOWERTANK 1

class PumpController
{

public:
    PumpController();
    void init(),
        run();
    // Getters
    int getLowerTankLevel(),
        getUpperTankLevel(),
        getLowerTankMin(),
        getUpperTankMin();
    TankParameters *getLowerTank(),
        *getUpperTank();

protected:
    TankParameters upperTank,
        lowerTank;
    int getTankLevel(int tank);
    TankParameters tanks[N_SENSORS];
};
