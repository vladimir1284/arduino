#include "pump_controller.h"

int ECHO_PINS[N_SENSORS] = {PA8, PA9}; // Echo pins

PumpController::PumpController()
{
    lowerTank = TankParameters(BASE_ADR_LOWER_TANK);
    upperTank = TankParameters(BASE_ADR_UPPER_TANK);
    tanks[UPPERTANK] = upperTank;
    tanks[LOWERTANK] = lowerTank;
}

void PumpController::run()
{
    readSR04sensors();
}

int PumpController::getLowerTankMin()
{
    return lowerTank.getMin();
}

int PumpController::getUpperTankMin()
{
    return upperTank.getMin();
}

TankParameters* PumpController::getLowerTank()
{
    return &lowerTank;
}

TankParameters* PumpController::getUpperTank()
{
    return &upperTank;
}


int PumpController::getUpperTankLevel()
{
    return getTankLevel(UPPERTANK);
}

int PumpController::getLowerTankLevel()
{
    return getTankLevel(LOWERTANK);
}

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

void PumpController::init()
{
    setupUltrasonicSensors(triggerPIN, N_SENSORS, ECHO_PINS);
}