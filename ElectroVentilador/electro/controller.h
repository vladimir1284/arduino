#ifndef controller_h
#define controller_h

#include "configs.h"
#include "mean_filter.h"
#include "signal.h"

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
        //setTemperature(int temp),
        run(int temp),
        init(int ntcPin, int buzzerPin, int speed0Pin,
             int speed1Pin, int temp1, int hysteresis,
             int calibration, int VoltCalibration,
             int sensorAPIN, int sensorBPIN,
             int voltagePIN, int actBuzzPIN),
        setTemp1(int temp),
        setHysteresis(int hyst),
        setCalibration(int cal),
        setVoltCalibration(int cal),
        turnOnBuzzer(),
        turnOffBuzzer();

    int getErrorCode(),
        getFanSpeed();

    bool getOverTemperature(),
        getACstatus(),
        getOverPressure();

    float getTemperature(),
        getVoltage();

private:
    int pinNTC,
        pinVOLT,
        pinBuzzer,
        pinBuzzAct,
        pinSpeed0,
        pinSpeed1,
        temp1,
        hysteresis,
        calibration,
        voltCalibration,
        error_code;

    float voltage,
        temperature;

    states state;
    MeanFilter TempFilter,
        voltFiler;

    ExternalSignal signalA,
        signalB;

    bool buzzerState,
        sound;

    // bool signalAisActive(),
    //      signalBisActive();

    unsigned int lastBuzzerChange;

    void trunOnSpeed0(),
        trunOffSpeed0(),
        trunOnSpeed1(),
        trunOffSpeed1(),
        readTemperature(),
        readVoltage(),
        handleBuzzer(),
        handleOverVoltage();
};
#endif