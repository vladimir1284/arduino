
#include "configs.h"
#include "mean_filter.h"

enum states
{
    IDLE,
    SPEED0,
    SPEED1,
    ALARM
};

class ElectroController
{

public:
    ElectroController();

    void run(),
        run(int temp),
        init(int ntcPin, int buzzerPin, int speed0Pin,
             int speed1Pin, int temp1, int hysteresis,
             int calibration),
        setTemp1(int temp),
        setHysteresis(int hyst),
        setCalibration(int cal);

    int getTemperature(),
        getErrorCode(),
        getFanSpeed();

private:
    int pinNTC,
        pinBuzzer,
        pinSpeed0,
        pinSpeed1,
        temp1,
        hysteresis,
        calibration,
        error_code,
        temperature;

    states state;
    MeanFilter filter;

    int readTemperature();

    void turnOnBuzzer(),
        turnOffBuzzer(),
        trunOnSpeed0(),
        trunOffSpeed0(),
        trunOnSpeed1(),
        trunOffSpeed1();
};