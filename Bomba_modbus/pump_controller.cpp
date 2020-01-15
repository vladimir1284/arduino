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
        if (getUpperTankLevel() < upperTank.getRestart()
            && getLowerTankLevel() > lowerTank.getMin())
        {
            startPump();
            currenState = PUMP_ON;
        }
        break;

    case PUMP_ON:
        if (getUpperTankLevel() > FULL_TANK){
            pumpStop();
            currenState = PUMP_IDLE;
        }
        if (getLowerTankLevel() < lowerTank.getMin()){
            pumpStop();
            currenState = PUMP_WAIT;
        }
        break;

    case PUMP_WAIT:
        if (getLowerTankLevel() > lowerTank.getRestart()){
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
    int xTicksStartDelay = pdMS_TO_TICKS(START_CAP_DELAY);
    // Time needed for desconecting the start capacitor
    int xTicksPumpOnDelay = pdMS_TO_TICKS(PUMP_ON_DELAY);

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
void PumpController::init()
{
    currenState = PUMP_IDLE;

    pinMode(pumpPIN, OUTPUT);
    pinMode(capacitorPIN, OUTPUT);
    digitalWrite(pumpPIN, LOW);
    digitalWrite(capacitorPIN, LOW);

    setupUltrasonicSensors(triggerPIN, N_SENSORS, ECHO_PINS);

    lowerTank.init(BASE_ADR_LOWER_TANK);
    upperTank.init(BASE_ADR_UPPER_TANK);
    tanks[UPPERTANK] = upperTank;
    tanks[LOWERTANK] = lowerTank;
}