
#include <Arduino.h>
#include "configs.h"
#include "mean_filter.h"

#define DUAL false
#define DUAL_HYST_FACTOR 2
#define TMAX 105


enum states
{
    IDLE,
    SPEED0,
    SPEED1,
    ALARM
};

states state;

class ElectroController
{

public:
    ElectroController();

    void run(),
        run(int temp),
        init(int ntcPin, int buzzerPin, int speed0Pin,
             int speed1Pin, int temp1, int hysteresis,
             int calibration);

    int getTemperature(),
        getFanSpeed();

private:
    int pinNTC,
        pinBuzzer,
        pinSpeed0,
        pinSpeed1,
        temp1,
        hysteresis,
        calibration,
        temperature;
    
    MeanFilter filter;

    int readTemperature();

    void turnOnBuzzer(),
        turnOffBuzzer(),
        trunOnSpeed0(),
        trunOffSpeed0(),
        trunOnSpeed1(),
        trunOffSpeed1(),
        setTemp1(int temp),
        setHysteresis(int hyst),
        setCalibration(int cal);
};