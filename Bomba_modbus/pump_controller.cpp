#include "pump_controller.h"

int ECHO_PINS[N_SENSORS] = {PA8, PA9}; // Echo pins

// Constructor
PumpController::PumpController()
{
    lowerTank = TankParameters();
    upperTank = TankParameters();
}

//----------------------------------------------------------------------------------
void PumpController::errorAck()
{
    // Set current state to IDLE on error Acknowledge
    currenState = PUMP_IDLE;
}

//----------------------------------------------------------------------------------
void PumpController::startPump()
{
    xTaskCreate(vPumpStartTask,
                "TaskPumpStart",
                configMINIMAL_STACK_SIZE,
                this,
                tskIDLE_PRIORITY,
                &xPumpStartTaskHandle);
}

//----------------------------------------------------------------------------------
void PumpController::run()
{
    // Update sensor values
    readSR04sensors();

    // Main FSM
    switch (currenState)
    {
    case PUMP_IDLE:
        if (getUpperTankLevel() < upperTank.getRestart() && getLowerTankLevel() > lowerTank.getMin())
        {
            startPump();
            currenState = PUMP_ON;
        }
        break;

    case PUMP_ON:
        if (getUpperTankLevel() > fullTank)
        {
            pumpStop();
            currenState = PUMP_IDLE;
        }
        if (getLowerTankLevel() < lowerTank.getMin())
        {
            pumpStop();
            currenState = PUMP_WAIT;
        }
        break;

    case PUMP_WAIT:
        if (getLowerTankLevel() > lowerTank.getRestart())
        {
            startPump();
            currenState = PUMP_ON;
        }
        break;

    case PUMP_ERROR:
        /* code */
        break;

    default:
        break;
    }
}

//----------------------------------------------------------------------------------
void PumpController::pumpStop()
{
    // Stop pump
    digitalWrite(pumpPIN, LOW);
    digitalWrite(capacitorPIN, LOW);

    // Kill starting task
    vTaskDelete(xPumpStartTaskHandle);
}

//----------------------------------------------------------------------------------
void PumpController::vPumpStartTask(void *pvParameters)
{
    PumpController *pumpCtrl = (PumpController *)pvParameters;

    TickType_t xLastWakeTime;
    // Time needed for desconecting the start capacitor
    int xTicksStartDelay = pdMS_TO_TICKS(START_CAP_FACTOR * pumpCtrl->getStartDelay());
    // Time needed for desconecting the start capacitor
    int xTicksPumpOnDelay = pdMS_TO_TICKS(PUMP_ON_FACTOR * pumpCtrl->getOnDelay());

    xLastWakeTime = xTaskGetTickCount();

    // Start
    digitalWrite(pumpPIN, HIGH);
    digitalWrite(capacitorPIN, HIGH);

    // Wait for starting
    vTaskDelayUntil(&xLastWakeTime, xTicksStartDelay);

    // Disconnect capacitor
    digitalWrite(capacitorPIN, LOW);

    // Wait for the maximum pumping time
    vTaskDelayUntil(&xLastWakeTime, xTicksPumpOnDelay);

    pumpCtrl->currenState = PUMP_ERROR;
    pumpCtrl->pumpStop();
}

//----------------------------------------------------------------------------------
int PumpController::getLowerTankMin()
{
    return lowerTank.getMin();
}

//----------------------------------------------------------------------------------
int PumpController::getUpperTankMin()
{
    return upperTank.getMin();
}

//----------------------------------------------------------------------------------
TankParameters *PumpController::getLowerTank()
{
    return &lowerTank;
}

//----------------------------------------------------------------------------------
TankParameters *PumpController::getUpperTank()
{
    return &upperTank;
}

//----------------------------------------------------------------------------------
int PumpController::getUpperTankLevel()
{
    return getTankLevel(UPPERTANK);
}

//----------------------------------------------------------------------------------
int PumpController::getLowerTankLevel()
{
    return getTankLevel(LOWERTANK);
}

//----------------------------------------------------------------------------------
int PumpController::getTankLevel(int tank)
{
    int level = getLevel(tank);              // Distance between sensor and surface
    level = tanks[tank].getHeight() - level; // Distance from the bottom
    level = 100 * level /
            (tanks[tank].getHeight() - tanks[tank].getGap()); // % of the level
    if (level < 0)
    {
        level = 0;
    }
    if (level > 100)
    {
        level = 100;
    }
    return level;
}

//----------------------------------------------------------------------------------
int PumpController::setOnDelay(int newValue)
{
    if (newValue >= MIN_ON && newValue <= MAX_ON)
    {
        onDelay = newValue;
        saveInt(ADR_PUMP_ON, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

//----------------------------------------------------------------------------------
int PumpController::setStartDelay(int newValue)
{
    if (newValue >= MIN_DELAY && newValue <= MAX_DELAY)
    {
        startDelay = newValue;
        saveInt(ADR_PUMP_START_CAP, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

//----------------------------------------------------------------------------------
int PumpController::setFullTank(int newValue)
{
    if (newValue >= MIN_FULL && newValue <= MAX_FULL)
    {
        fullTank = newValue;
        saveInt(ADR_FULL_TANK, newValue);
        return 0;
    }
    else
    {
        return -1;
    }
}

//----------------------------------------------------------------------------------
int PumpController::getOnDelay()
{
    return onDelay;
}

//----------------------------------------------------------------------------------
int PumpController::getStartDelay()
{
    return startDelay;
}

//----------------------------------------------------------------------------------
int PumpController::getFullTank()
{
    return fullTank;
}

//----------------------------------------------------------------------------------
void PumpController::init()
{
    currenState = PUMP_IDLE;

    pinMode(pumpPIN, OUTPUT);
    pinMode(capacitorPIN, OUTPUT);
    digitalWrite(pumpPIN, LOW);
    digitalWrite(capacitorPIN, LOW);

    setupUltrasonicSensors(triggerPIN, N_SENSORS, ECHO_PINS);

    lowerTank.init(ADR_LT_HEIGTH, ADR_LT_GAP, ADR_LT_RESTART, ADR_LT_MIN);
    upperTank.init(ADR_UT_HEIGTH, ADR_UT_GAP, ADR_UT_RESTART, ADR_UT_MIN);
    tanks[UPPERTANK] = upperTank;
    tanks[LOWERTANK] = lowerTank;

    onDelay = loadInt(ADR_PUMP_ON);
    startDelay = loadInt(ADR_PUMP_START_CAP);
    fullTank = loadInt(ADR_FULL_TANK);

    // Verify and Auto-correct
    if (onDelay < MIN_ON || onDelay > MAX_ON)
    {
        setOnDelay(MAX_ON);
    }
    if (startDelay < MIN_DELAY || startDelay > MAX_DELAY)
    {
        setStartDelay(MAX_DELAY);
    }
    if (fullTank < MIN_FULL || fullTank > MAX_FULL)
    {
        setFullTank(MAX_FULL - 10);
    }
}