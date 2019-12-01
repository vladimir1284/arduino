#include "controller.h"

// Constructor
ElectroController::ElectroController()
{
    TempFilter = MeanFilter();
    voltFiler = MeanFilter();
    signalA = ExternalSignal();
    signalB = ExternalSignal();
}

//--------------------------------------------------------------------
void ElectroController::run()
{
    readTemperature();
    readVoltage();
    run(temperature);
}

//--------------------------------------------------------------------
void ElectroController::handleOverVoltage()
{
    if ((voltage <= VOLT_MAX) && (state != ALARM))
    {
        turnOffBuzzer();
    }
    if ((voltage > VOLT_ALARM) && !buzzerState)
    {
        turnOnBuzzer();
    }
}

//--------------------------------------------------------------------
void ElectroController::handleBuzzer()
{
    // Over voltage Monitor
    handleOverVoltage();
    // Pulse generation
    if (buzzerState)
    {
        if (sound)
        {
            if (millis() - lastBuzzerChange > BUZZER_ON_DELAY)
            {
                lastBuzzerChange = millis();
                noTone(pinBuzzer);
                sound = false;
            }
        }
        else
        {
            if (millis() - lastBuzzerChange > BUZZER_OFF_DELAY)
            {
                lastBuzzerChange = millis();
                tone(pinBuzzer, BUZZ_FREQ);
                sound = true;
            }
        }
    }
}

//--------------------------------------------------------------------
void ElectroController::run(int temp)
{
    // Handle buzzer
    handleBuzzer();
    // Monitor external signals
    signalA.run();
    signalB.run();
    // Main FSM
    switch (state)
    {
    case IDLE:
        // Tasks
        trunOffSpeed0();

        // Next state
        if (temp >= temp1 ||
            signalA.isActive() ||
            signalB.isActive())
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
            !signalA.isActive() &&
            !signalB.isActive())
        {
            state = IDLE;
        }
        if (temp >= (temp1 + hysteresis) ||
            signalB.isActive())
        {
            state = SPEED1;
        }
        break;

    case SPEED1:
        // Tasks
        trunOnSpeed1();

        // Next state
        if (temp <= temp1 &&
            !signalB.isActive())
        {
            state = SPEED0;
        }
        if (temp >= (temp1 + 2 * hysteresis))
        {
            state = ALARM;
            turnOnBuzzer();
        }
        break;

    case ALARM:
        // Tasks

        // Next state
        if (temp <= (temp1 + 2 * hysteresis - 1))
        {
            state = SPEED1;
            turnOffBuzzer();
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
                             int voltagePIN, int actBuzzPIN)
{
    // Configure IO pins
    pinNTC = ntcPin;
    pinMode(pinNTC, INPUT_ANALOG);

    pinVOLT = voltagePIN;
    pinMode(pinVOLT, INPUT_ANALOG);

    pinBuzzer = buzzerPin;
    pinMode(pinBuzzer, OUTPUT);

    pinBuzzAct = actBuzzPIN;
    pinMode(pinBuzzAct, OUTPUT);

    pinSpeed0 = speed0Pin;
    pinMode(pinSpeed0, OUTPUT);

    pinSpeed1 = speed1Pin;
    pinMode(pinSpeed1, OUTPUT);

    signalA.init(sensorAPIN);
    signalB.init(sensorBPIN);

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

//--------------------------------------------------------------------
void ElectroController::turnOnBuzzer()
{
    if (!buzzerState)
    {
        digitalWrite(pinBuzzAct, HIGH);
        buzzerState = HIGH;
        tone(pinBuzzer, BUZZ_FREQ);
        sound = true;
    }
}

//--------------------------------------------------------------------
void ElectroController::turnOffBuzzer()
{
    if (buzzerState)
    {
        digitalWrite(pinBuzzAct, LOW);
        buzzerState = LOW;
        noTone(pinBuzzer);
        sound = false;
    }
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

//--------------------------------------------------------------------
bool ElectroController::getOverTemperature()
{
    return state == ALARM;
}

//--------------------------------------------------------------------
bool ElectroController::getOverPressure()
{
    return signalB.isActive();
}

//--------------------------------------------------------------------
bool ElectroController::getACstatus()
{
    return signalA.isActive();
}