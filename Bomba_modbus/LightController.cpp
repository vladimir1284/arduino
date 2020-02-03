#include "LightController.h"

// ----- Constructor -------
LightController::LightController()
{
}

// ----------------------------------------------------------------------------
void LightController::init(int relayPin, int relayOn, int pirPin, int pirOn,
                           int luminosityPin, int luminosityMonotony,
                           int addrMode, int addrSleepTime, int addrSmart, int addrSmartDelay,
                           int addrInitDelay, int addrDelayIncrement, int addrThreshold)
{
    relay_on = relayOn;
    pir_pin = pirPin;
    pir_on = pirOn;
    luminosity_pin = luminosityPin;
    luminosity_monotony = luminosityMonotony;
    //
    config(relayPin, addrMode, addrSleepTime, addrSmart, addrSmartDelay,
           addrInitDelay, addrDelayIncrement, addrThreshold);
}

// ----------------------------------------------------------------------------
void LightController::init(int relayPin, int pirPin,
                           int luminosityPin,
                           int addrMode, int addrSleepTime, int addrSmart, int addrSmartDelay,
                           int addrInitDelay, int addrDelayIncrement, int addrThreshold)
{
    relay_on = HIGH;
    pir_pin = pirPin;
    pir_on = HIGH;
    luminosity_pin = luminosityPin;
    luminosity_monotony = 1;
    //
    config(relayPin, addrMode, addrSleepTime, addrSmart, addrSmartDelay,
           addrInitDelay, addrDelayIncrement, addrThreshold);
}

// ----------------------------------------------------------------------------
void LightController::config(int relayPin, int addrMode, int addrSleepTime, int addrSmart, int addrSmartDelay,
                             int addrInitDelay, int addrDelayIncrement, int addrThreshold)
{
    relay_pin = relayPin;
    addr_mode = addrMode;
    addr_sleepTime = addrSleepTime;
    addr_smart = addrSmart;
    addr_smartDelay = addrSmartDelay;
    addr_initDelay = addrInitDelay;
    addr_delayIncrement = addrDelayIncrement;
    addr_threshold = addrThreshold;

    // Load from EEPROM
    mode = (modes)loadInt(addr_mode);
    smart = loadInt(addr_smart);
    sleepTime = loadInt(addr_sleepTime);
    smartDelay = loadInt(addr_smartDelay);
    initDelay = loadInt(addr_initDelay);
    delayIncrement = loadInt(addr_delayIncrement);
    luminosityThreshold = loadInt(addr_threshold);

    // Verify and Auto-correct
    if (mode < MIN_MODE || mode > MAX_MODE)
    {
        setMode(MIN_MODE);
    }
    if (sleepTime < MIN_SLEEP_TIME || sleepTime > MAX_SLEEP_TIME)
    {
        setSleepTime(MIN_SLEEP_TIME);
    }
    if (smart < 0)
    {
        setSmart(1);
    }
    if (smartDelay < MIN_SMART_DELAY || smartDelay > MAX_SMART_DELAY)
    {
        setSmartDelay(MIN_SMART_DELAY);
    }
    if (initDelay < MIN_INIT_DELAY || initDelay > MAX_INIT_DELAY)
    {
        setInitDelay(MIN_INIT_DELAY);
    }
    if (luminosityThreshold < MIN_THRESHOLD || luminosityThreshold > MAX_THRESHOLD)
    {
        setThreshold(50);
    }

    // Config Pins
    if (pir_pin != -1)
    {
        pinMode(pir_pin, INPUT);
    }
    else
    {
        pir_state = NC_VALUE;
    }
    if (luminosity_pin != -1)
    {
        pinMode(luminosity_pin, INPUT_ANALOG);
    }
    else
    {
        luminosity = NC_VALUE;
    }
    pinMode(relay_pin, OUTPUT);

    // Initial values
    delayOn = initDelay;
    currentState = ACTIVE_OFF;
    lastSatisfied = false;
}

// ----------------------------------------------------------------------------
bool LightController::thresholdSatisfied()
{
    if (lastSatisfied)
    {
        if (luminosity < (luminosityThreshold - THRESHOLD_HYST))
        {
            lastSatisfied = false;
        }
    }
    else
    {
        if (luminosity > (luminosityThreshold + THRESHOLD_HYST))
        {
            lastSatisfied = true;
        }
    }
    return lastSatisfied;
}

// ----------------------------------------------------------------------------
void LightController::scanSensors()
{
    if (pir_pin != -1)
    {
        pir_state = (digitalRead(pir_pin) == pir_on);
    }
    if (luminosity_pin != -1)
    {
        luminosity = 100 * analogRead(luminosity_pin) / MAX_ANALOG_READ;
        // Fix for decreasing monotony
        if (luminosity_monotony < 0)
        {
            luminosity = 100 - luminosity;
        }
    }
}

// ----------------------------------------------------------------------------
void LightController::turnOnLight()
{
    light_state = HIGH;
    digitalWrite(relay_pin, relay_on);
}

// ----------------------------------------------------------------------------
void LightController::turnOffLight()
{
    light_state = LOW;
    digitalWrite(relay_pin, !relay_on);
}

// ----------------------------------------------------------------------------
void LightController::run()
{
    scanSensors();
    switch (mode)
    {
    case ON:
        turnOnLight();
        break;
    case INACTIVE:
        turnOffLight();
        break;
    case ACTIVE:
        handleActiveMode();
        break;
    case COURTESY:
        handleCourtesyMode();
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
void LightController::handleActiveMode()
{
    switch (currentState)
    {
    case SLEEPING:
        if (millis() - timeStamp > sleepTime * MIN2MSEC)
        {
            currentState = ACTIVE_OFF;
        }
        break;
    case ACTIVE_OFF:
        if (thresholdSatisfied() && pir_state == true)
        {
            currentState = ACTIVE_ON;
            turnOnLight();
            timeStamp = millis();
        }
        break;
    case ACTIVE_ON:
        if (millis() - timeStamp > delayOn * MIN2MSEC)
        {
            if (smart)
            {
                currentState = ACTIVE_OFF1;
                turnOffLight();
                timeStamp = millis();
            }
            else
            {
                currentState = ACTIVE_OFF;
                turnOffLight();
            }
        }
        break;
    case ACTIVE_OFF1:
        if (pir_state == true)
        {
            currentState = ACTIVE_ON;
            turnOnLight();
            timeStamp = millis();
            delayOn += delayIncrement;
        }
        if (millis() - timeStamp > smartDelay * 1000)
        {
            delayOn = initDelay;
            currentState = ACTIVE_OFF;
        }
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
void LightController::handleCourtesyMode()
{
    switch (currentState)
    {
    case SLEEPING:
        if (millis() - timeStamp > sleepTime * MIN2MSEC)
        {
            currentState = ACTIVE_OFF;
        }
        break;
    case ACTIVE_OFF:
        if (thresholdSatisfied())
        {
            currentState = ACTIVE_ON;
            turnOnLight();
        }
        break;
    case ACTIVE_ON:
        if (!thresholdSatisfied())
        {
            currentState = ACTIVE_OFF;
            turnOffLight();
        }
        break;
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
void LightController::startSleep()
{
    timeStamp = millis();
    currentState = SLEEPING;
}

// ----------------------------------------------------------------------------
int LightController::getLight_state() { return light_state; }

// ----------------------------------------------------------------------------
int LightController::getPir_state() { return pir_state; }

// ----------------------------------------------------------------------------
int LightController::getLuminosity() { return luminosity; }

// ----------------------------------------------------------------------------
int LightController::getMode() { return mode; }

// ----------------------------------------------------------------------------
int LightController::getSleepTime() { return sleepTime; }

// ----------------------------------------------------------------------------
int LightController::getSmart() { return smart; }

// ----------------------------------------------------------------------------
int LightController::getSmartDelay() { return smartDelay; }

// ----------------------------------------------------------------------------
int LightController::getInitDelay() { return initDelay; }

// ----------------------------------------------------------------------------
int LightController::getDelayIncrement() { return delayIncrement; }

// ----------------------------------------------------------------------------
int LightController::getThreshold() { return luminosityThreshold; }

// ----------------------------------------------------------------------------
int LightController::setMode(int newValue)
{
    if (newValue >= MIN_MODE && newValue <= MAX_MODE)
    {
        if (luminosity_pin == -1)
        {
            // Cannot be courtesy light
            if (newValue == COURTESY)
            {
                return -1;
            }
        }
        if (pir_pin == -1)
        {
            // Cannot be in active mode
            if (newValue == ACTIVE)
            {
                return -1;
            }
        }
        mode = (modes)newValue;
        saveInt(addr_mode, mode);
        return 0;
    }
    else
    {
        return -1;
    }
}

// ----------------------------------------------------------------------------
int LightController::setSleepTime(int newValue)
{
    if (newValue >= MIN_SLEEP_TIME && newValue <= MAX_SLEEP_TIME)
    {
        sleepTime = newValue;
        saveInt(addr_sleepTime, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

// ----------------------------------------------------------------------------
int LightController::setSmart(int newValue)
{
    if (newValue >= false && newValue <= true)
    {
        smart = newValue;
        saveInt(addr_smart, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

// ----------------------------------------------------------------------------
int LightController::setSmartDelay(int newValue)
{
    if (newValue >= MIN_SMART_DELAY && newValue <= MAX_SMART_DELAY)
    {
        smartDelay = newValue;
        saveInt(addr_smartDelay, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

// ----------------------------------------------------------------------------
int LightController::setInitDelay(int newValue)
{
    if (newValue >= MIN_INIT_DELAY && newValue <= MAX_INIT_DELAY)
    {
        initDelay = newValue;
        saveInt(addr_initDelay, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

// ----------------------------------------------------------------------------
int LightController::setDelayIncrement(int newValue)
{
    if (newValue >= MIN_DELAY_INCREMENT && newValue <= MAX_DELAY_INCREMENT)
    {
        delayIncrement = newValue;
        saveInt(addr_delayIncrement, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

// ----------------------------------------------------------------------------
int LightController::setThreshold(int newValue)
{
    if (newValue >= MIN_THRESHOLD && newValue <= MAX_THRESHOLD)
    {
        luminosityThreshold = newValue;
        saveInt(addr_threshold, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}