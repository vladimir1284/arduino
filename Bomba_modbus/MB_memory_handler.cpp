#include "MB_memory_handler.h"

// Constructor
MemoryHandler::MemoryHandler(PumpController *pumpController)
{
    pumpCtrl = pumpController;
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
    case ADR_PUMP:
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
    case ADR_PUMP:
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
        if (!pumpCtrl->getUpperTank()->setHeight(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_UT_GAP:
        if (!pumpCtrl->getUpperTank()->setGap(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_UT_MIN:
        if (!pumpCtrl->getUpperTank()->setMin(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_UT_RESTART:
        if (!pumpCtrl->getUpperTank()->setRestart(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_LT_LEVEL:
        return ERROR_ILEGAL_ADR;
        break;

    case ADR_LT_HEIGTH:
        if (!pumpCtrl->getLowerTank()->setHeight(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_LT_GAP:
        if (!pumpCtrl->getLowerTank()->setGap(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_LT_MIN:
        if (!pumpCtrl->getLowerTank()->setMin(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    case ADR_LT_RESTART:
        if (!pumpCtrl->getLowerTank()->setRestart(value)){
            return OK_ACTION;
        } else {
            return ERROR_INCORRECT_VALUE;
        }
        break;

    default:
        return ERROR_UNKNOWN_ADR;
        break;
    }
}