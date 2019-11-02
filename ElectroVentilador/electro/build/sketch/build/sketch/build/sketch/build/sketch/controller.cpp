#include "controller.h"

// Constructor
ElectroController::ElectroController()
{
    filter = MeanFilter();
}

//--------------------------------------------------------------------
void ElectroController::run()
{
    run(readTemperature());
}

//--------------------------------------------------------------------
void ElectroController::run(int temp)
{
    switch (state)
    {
    case IDLE:
        // Tasks
        trunOffSpeed0();

        // Next state
        if (temperature > (temp1 + (float)hysteresis / 2))
        {
            state = SPEED0;
        }
        break;

    case SPEED0:
        // Tasks
        trunOnSpeed0();

        // Next state
        if (temperature < (temp1 - (float)hysteresis / 2))
        {
            state = IDLE;
        }
        if (temperature > (temp1 + DUAL_HYST_FACTOR * (float)hysteresis / 2))
        {
            state = SPEED1;
        }
        break;

    case SPEED1:
        // Tasks
        trunOnSpeed1();
        turnOffBuzzer();

        // Next state
        if (temperature < temp1)
        {
            state = SPEED0;
        }
        if (temperature > TMAX)
        {
            state = ALARM;
        }
        break;

    case ALARM:
        // Tasks
        turnOnBuzzer();

        // Next state
        if (temperature < TMAX)
        {
            state = SPEED1;
        }
        break;

    default:
        break;
    }
}

//--------------------------------------------------------------------
void ElectroController::init(int ntcPin, int buzzerPin, int speed0Pin,
                             int speed1Pin, int temp1, int hysteresis,
                             int calibration)
{
    // Configure IO pins
    pinNTC = ntcPin;
    pinMode(pinNTC, INPUT_ANALOG);

    pinBuzzer = buzzerPin;
    pinMode(pinBuzzer, OUTPUT);

    pinSpeed0 = speed0Pin;
    pinMode(pinSpeed0, OUTPUT);

    pinSpeed1 = speed1Pin;
    pinMode(pinSpeed1, OUTPUT);

    // Set configurations
    setTemp1(temp1);
    setHysteresis(hysteresis);
    setCalibration(calibration);

    // Turn off outputs
    turnOffBuzzer();
    trunOffSpeed0();
    if (DUAL)
    {
        trunOffSpeed1();
    }

    // Set initial state
    state = IDLE;
}

//--------------------------------------------------------------------
/*************
 * Routine for obtainig the temperature from NTC
 * It will take into account the correction factor
 * provided by the user in configurations
 ************/
int ElectroController::readTemperature()
{
    int val = analogRead(pinNTC);
    double Temp;
    Temp = log(((40960000 / val) - 10000));
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
    Temp = Temp - 273.15; // Converierte de Kelvin a Celsius
    filter.insertValue(round(Temp + calibration));
    //temperature = filter.getValue();
    temperature = (int)Temp;
    return temperature;
}

//--------------------------------------------------------------------
int ElectroController::getTemperature()
{
    return temperature;
}

//--------------------------------------------------------------------
void ElectroController::turnOnBuzzer()
{
    digitalWrite(pinBuzzer, HIGH);
}

//--------------------------------------------------------------------
void ElectroController::turnOffBuzzer()
{
    digitalWrite(pinBuzzer, LOW);
}

//--------------------------------------------------------------------
void ElectroController::trunOnSpeed0()
{
    if (DUAL)
    {
        trunOffSpeed1();
    }
    digitalWrite(pinSpeed0, HIGH);
}

//--------------------------------------------------------------------
void ElectroController::trunOffSpeed0()
{
    digitalWrite(pinSpeed0, LOW);
}

//--------------------------------------------------------------------
void ElectroController::trunOnSpeed1()
{
    if (DUAL)
    {
        trunOffSpeed0();
        digitalWrite(pinSpeed1, HIGH);
    }
}

//--------------------------------------------------------------------
void ElectroController::trunOffSpeed1()
{
    if (DUAL)
    {
        digitalWrite(pinSpeed1, LOW);
    }
}

//--------------------------------------------------------------------
void ElectroController::setTemp1(int temp)
{
    temp1 = temp;
}

//--------------------------------------------------------------------
void ElectroController::setHysteresis(int hyst)
{
    hysteresis = hyst;
}

//--------------------------------------------------------------------
void ElectroController::setCalibration(int cal)
{
    calibration = cal;
}

//--------------------------------------------------------------------
int ElectroController::getFanSpeed()
{
    if (state == IDLE)
    {
        return FAN_STOP;
    }
    else if (state == SPEED0)
    {
        return FAN_SPEED_1;
    }
    else if (state == SPEED1 || state == ALARM)
    {
        if (DUAL)
        {
            return FAN_SPEED_2;
        }
        else
        {
            return FAN_SPEED_1;
        }
    }
}