#ifndef pump_controller_h
#define pump_controller_h

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
#include "configs.h"
#include "SR04.h"
#include <MapleFreeRTOS900.h>

#define MIN_DELAY 3 // seg
#define MAX_DELAY 5 // seg
#define MIN_ON 1    // min
#define MAX_ON 60   // min
#define MIN_FULL 30 // %
#define MAX_FULL 100// %


enum pump_states
{
    PUMP_IDLE,
    PUMP_ON,
    PUMP_WAIT,
    PUMP_ERROR,
};

class PumpController
{

public:
    PumpController();
    void init(),
        errorAck(),
        run();
    // Getters
    int getLowerTankLevel(),
        getUpperTankLevel(),
        getLowerTankMin(),
        getUpperTankMin(),
        getOnDelay(),
        getStartDelay(),
        getFullTank(),
        getPumpState();

    TankParameters *getLowerTank(),
        *getUpperTank();

    pump_states currenState;

    // Setters
    int setOnDelay(int newValue),
        setStartDelay(int newValue),
        setFullTank(int newValue),
        setPumpState(int newSate);

private:
    TankParameters upperTank,
        lowerTank;
    TaskHandle_t xPumpStartTaskHandle;
    TankParameters tanks[N_SENSORS];

    int getTankLevel(int tank);
    void pumpStop(),
        startPump();
    static void vPumpStartTask(void *pvParameters);

    int onDelay,    // Maximum time that the pump can be running (min)
        startDelay, // Delay for the strating capacitor to be connected
        fullTank;   // Percent of the upper tank to be considered full
};
#endif // !pump_controller_h