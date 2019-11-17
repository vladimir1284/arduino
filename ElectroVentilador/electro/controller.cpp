#include "controller.h"

// Constructor
ElectroController::ElectroController()
{
    TempFilter = MeanFilter();
    voltFiler = MeanFilter();
}

//--------------------------------------------------------------------
void ElectroController::run()
{
    readTemperature();
    readVoltage();
    run(temperature);
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
                             int calibration, int VoltCalibration,
                             int sensorAPIN, int sensorBPIN,
                             int voltagePIN)
{
    // Configure IO pins
    pinNTC = ntcPin;
    pinMode(pinNTC, INPUT_ANALOG);

    pinVOLT = voltagePIN;
    pinMode(pinVOLT, INPUT_ANALOG);

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
    setVoltCalibration(VoltCalibration);

    // Turn off outputs
    buzzerState = LOW;
    turnOffBuzzer();
    trunOffSpeed0();
    trunOffSpeed1();

    // Set initial state
    state = IDLE;
    buzzerState = LOW;
    sound = false;

    // Init temperature and voltage
    //voltage     = 0;
    //temperature = 0;
}

//--------------------------------------------------------------------
/*************
 * Routine for obtainig the batery voltage
 * It will take into account the correction factor
 * provided by the user in configurations
 ************/
void ElectroController::readVoltage()
{
    int val = analogRead(pinVOLT);
    float volt = (val * VOLT_FACTOR + (float)voltCalibration / 10);

    voltFiler.insertValue(volt);
    //voltage = volt;
    voltage = voltFiler.getValue();
}

//--------------------------------------------------------------------
/*************
 * Routine for obtainig the temperature from NTC
 * It will take into account the correction factor
 * provided by the user in configurations
 ************/
void ElectroController::readTemperature()
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
        float Temp;
        Temp = log(((40960000 / val) - 10000));
        Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
        Temp = Temp - 273.15; // Converierte de Kelvin a Celsius
        TempFilter.insertValue(Temp + calibration);
        temperature = TempFilter.getValue();
    }
}

//--------------------------------------------------------------------
int ElectroController::getErrorCode()
{
    return error_code;
}

//--------------------------------------------------------------------
float ElectroController::getTemperature()
{
    return temperature;
}

//--------------------------------------------------------------------
float ElectroController::getVoltage()
{
    return voltage;
}

// //--------------------------------------------------------------------
// void ElectroController::setTemperature(int temp)
// {
//     temperature = temp;
// }

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
void ElectroController::setVoltCalibration(int cal)
{
    voltCalibration = cal;
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