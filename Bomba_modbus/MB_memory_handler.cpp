#include "MB_memory_handler.h"

// Constructor
MemoryHandler::MemoryHandler(PumpController *pumpController,
                             LightController *lamp0)
{
    pumpCtrl = pumpController;
    light0 = lamp0;
}

//----------------------------------------------------------------------------------
/**
 * @brief
 * This method gets the value of the given coil
 *
 * @return value (0-1) if the given coil is found, -1 otherwise. 
 * @ingroup coil
 */
int MemoryHandler::getCoilValue(int address)
{
    switch (address)
    {
    case ADR_ERROR_ACK:
        // TODO return real value
        return OK_ACTION;
        break;

    default:
        return ERROR_UNKNOWN_ADR;
        break;
    }
}

//----------------------------------------------------------------------------------
/**
 * @brief
 * This method gets the value of the given coil
 *
 * @return value (0-1) if the given coil is found, -1 otherwise. 
 * @ingroup coil
 */
int MemoryHandler::setCoilValue(int address, bool value)
{
    switch (address)
    {
    case ADR_ERROR_ACK:
        if (value)
        {
            pumpCtrl->errorAck();
        }
        return OK_ACTION;
        break;

    default:
        return ERROR_UNKNOWN_ADR;
        break;
    }
}

//----------------------------------------------------------------------------------
/**
 * @brief
 * This method gets the value of the given register
 *
 * @return  the value if the given register is found.
 *         -1 address was not found.
 * @ingroup coil
 */
int MemoryHandler::getRegValue(int address)
{
    switch (address)
    {
    case ADR_UT_LEVEL:
        return pumpCtrl->getUpperTankLevel();
        break;

    case ADR_UT_HEIGTH:
        return pumpCtrl->getUpperTank()->getHeight();
        break;

    case ADR_UT_GAP:
        return pumpCtrl->getUpperTank()->getGap();
        break;

    case ADR_UT_MIN:
        return pumpCtrl->getUpperTank()->getMin();
        break;

    case ADR_UT_RESTART:
        return pumpCtrl->getUpperTank()->getRestart();
        break;

    case ADR_LT_LEVEL:
        return pumpCtrl->getLowerTankLevel();
        break;

    case ADR_LT_HEIGTH:
        return pumpCtrl->getLowerTank()->getHeight();
        break;

    case ADR_LT_GAP:
        return pumpCtrl->getLowerTank()->getGap();
        break;

    case ADR_LT_MIN:
        return pumpCtrl->getLowerTank()->getMin();
        break;

    case ADR_LT_RESTART:
        return pumpCtrl->getLowerTank()->getRestart();
        break;

    case ADR_PUMP_START_CAP:
        return pumpCtrl->getStartDelay();
        break;

    case ADR_PUMP_ON:
        return pumpCtrl->getOnDelay();
        break;

    case ADR_FULL_TANK:
        return pumpCtrl->getFullTank();
        break;

    case ADR_LIGHT_MODE_0:
        return light0->getMode();
        break;

    case ADR_LIGHT_SLEEP_TIME_0:
        return light0->getSleepTime();
        break;

    case ADR_LIGHT_SMART_0:
        return light0->getSmart();
        break;

    case ADR_LIGHT_SMART_DELAY_0:
        return light0->getSmartDelay();
        break;

    case ADR_LIGHT_INIT_DELAY_0:
        return light0->getInitDelay();
        break;

    case ADR_LIGHT_DELAY_INCREMENT_0:
        return light0->getDelayIncrement();
        break;

    case ADR_LIGHT_TRESHOLD_0:
        return light0->getThreshold();
        break;

    case ADR_PC_STATE:
        return pumpCtrl->getPumpState();
        break;

    case ADR_LUMINOSITY_0:
        return light0->getLuminosity();
        break;

    case ADR_PIR_STATE_0:
        return light0->getPir_state();
        break;

    case ADR_LIGHT_STATE_0:
        return light0->getLight_state();
        break;

    default:
        return ERROR_UNKNOWN_ADR;
        break;
    }
}

//----------------------------------------------------------------------------------
/**
 * @brief
 * This method sets the value of the given register
 *
 * @return  0 if the value was corretly setted.
 *         -1 address was not found.
 *         -2 incorrect value. 
 *         -3 ilegal address. 
 * @ingroup coil
 */
int MemoryHandler::setRegValue(int address, uint16_t value)
{
    switch (address)
    {
    case ADR_UT_LEVEL:
        return ERROR_ILEGAL_ADR;
        break;

    case ADR_UT_HEIGTH:
        if (!pumpCtrl->getUpperTank()->setHeight(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_UT_GAP:
        if (!pumpCtrl->getUpperTank()->setGap(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_UT_MIN:
        if (!pumpCtrl->getUpperTank()->setMin(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_UT_RESTART:
        if (!pumpCtrl->getUpperTank()->setRestart(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LT_LEVEL:
        return ERROR_ILEGAL_ADR;
        break;

    case ADR_LT_HEIGTH:
        if (!pumpCtrl->getLowerTank()->setHeight(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LT_GAP:
        if (!pumpCtrl->getLowerTank()->setGap(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LT_MIN:
        if (!pumpCtrl->getLowerTank()->setMin(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LT_RESTART:
        if (!pumpCtrl->getLowerTank()->setRestart(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_PUMP_START_CAP:
        if (!pumpCtrl->setStartDelay(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_PUMP_ON:
        if (!pumpCtrl->setOnDelay(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_FULL_TANK:
        if (!pumpCtrl->setFullTank(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_MODE_0:
        if (!light0->setMode(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_SLEEP_TIME_0:
        if (!light0->setSleepTime(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_SMART_0:
        if (!light0->setSmart(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_SMART_DELAY_0:
        if (!light0->setSmartDelay(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_INIT_DELAY_0:
        if (!light0->setInitDelay(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_DELAY_INCREMENT_0:
        if (!light0->setDelayIncrement(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LIGHT_TRESHOLD_0:
        if (!light0->setThreshold(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_PC_STATE:
        if (!pumpCtrl->setPumpState(value))
        {
            return OK_ACTION;
        }
        break;

    case ADR_LUMINOSITY_0:
        return ERROR_ILEGAL_ADR;
        break;

    case ADR_PIR_STATE_0:
        return ERROR_ILEGAL_ADR;
        break;

    case ADR_LIGHT_STATE_0:
        return ERROR_ILEGAL_ADR;
        break;

    default:
        return ERROR_UNKNOWN_ADR;
        break;
    }
    return ERROR_INCORRECT_VALUE;
}