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
void ElectroController::handleBuzzer()
{
    if (buzzerState)
    {
        if (sound)
        {
            if (millis() - lastBuzzerChange > BUZZER_ON_DELAY)
            {
                lastBuzzerChange = millis();
                turnOffBuzzer();
            }
        }
        else
        {
            if (millis() - lastBuzzerChange > BUZZER_OFF_DELAY)
            {
                lastBuzzerChange = millis();
                turnOnBuzzer();
            }
        }
    }
    else
    {
        turnOffBuzzer();
    }
}

//--------------------------------------------------------------------
bool ElectroController::signalAisActive()
{
    return (digitalRead(pinSensorA) == LOW);
}

//--------------------------------------------------------------------
bool ElectroController::signalBisActive()
{
    return (digitalRead(pinSensorB) == LOW);
}

//--------------------------------------------------------------------
void ElectroController::run(int temp)
{
    // Handle buzzer
    handleBuzzer();
    // Main FSM
    switch (state)
    {
    case IDLE:
        // Tasks
        trunOffSpeed0();

        // Next state
        if (temp >= temp1 ||
            signalAisActive() ||
            signalBisActive())
        {
            state = SPEED0;
        }
        break;

    case SPEED0:
        // Tasks
        trunOffSpeed1();
        trunOnSpeed0();
        // Next state
        if ((temp <= (temp1 - hysteresis)) &&
            !signalAisActive() &&
            !signalBisActive())
        {
            state = IDLE;
        }
        if (temp >= (temp1 + hysteresis) ||
            signalBisActive())
        {
            state = SPEED1;
        }
        break;

    case SPEED1:
        // Tasks
        trunOnSpeed1();
        buzzerState = LOW;

        // Next state
        if (temp <= temp1 &&
            !signalBisActive())
        {
            state = SPEED0;
        }
        if (temp >= (temp1 + 2 * hysteresis))
        {
            state = ALARM;
        }
        break;

    case ALARM:
        // Tasks
        buzzerState = HIGH;

        // Next state
        if (temp <= (temp1 + 2 * hysteresis - 1))
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
                             int calibration, int sensorAPIN, int sensorBPIN)
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

    pinSensorA = sensorAPIN;
    pinMode(pinSensorA, INPUT_PULLUP);

    pinSensorB = sensorBPIN;
    pinMode(pinSensorB, INPUT_PULLUP);

    // Set configurations
    setTemp1(temp1);
    setHysteresis(hysteresis);
    setCalibration(calibration);

    // Turn off outputs
    buzzerState = LOW;
    turnOffBuzzer();
    trunOffSpeed0();
    trunOffSpeed1();

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
    int val = 4096 - analogRead(pinNTC);
    // Validate sensor conection
    if (val < MIN_VAL_CODE)
    {
        error_code = OPEN_CIRCUIT;
    }
    else if (val > MAX_VAL_CODE)
    {
        error_code = SHORT_CIRCUIT;
    }
    else
    {
        error_code = NO_ERROR;

        // Compute temperature
        double Temp;
        Temp = log(((40960000 / val) - 10000));
        Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
        Temp = Temp - 273.15; // Converierte de Kelvin a Celsius
        filter.insertValue(Temp + calibration);
        temperature = filter.getValue();
    }
    return temperature;
}

//--------------------------------------------------------------------
int ElectroController::getErrorCode()
{
    return error_code;
}

//--------------------------------------------------------------------
int ElectroController::getTemperature()
{
    return temperature;
}

//--------------------------------------------------------------------
void ElectroController::setTemperature(int temp)
{
    temperature = temp;
}

//--------------------------------------------------------------------
void ElectroController::turnOnBuzzer()
{
    //digitalWrite(pinBuzzer, HIGH);
    tone(pinBuzzer, 200);
    sound = true;
}

//--------------------------------------------------------------------
void ElectroController::turnOffBuzzer()
{
    //digitalWrite(pinBuzzer, LOW);
    noTone(pinBuzzer);
    sound = false;
}

//--------------------------------------------------------------------
void ElectroController::trunOnSpeed0()
{
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
    digitalWrite(pinSpeed1, HIGH);
}

//--------------------------------------------------------------------
void ElectroController::trunOffSpeed1()
{
    digitalWrite(pinSpeed1, LOW);
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
        return FAN_SPEED_2;
    }
}