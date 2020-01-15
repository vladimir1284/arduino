#ifndef memory_handler_h
#define memory_handler_h

#include "TankParameters.h"
#include "pump_controller.h"

#define REGISTER_SIZE 16 // Depends on reg_addresses

enum error_codes
{
    OK_ACTION,
    ERROR_UNKNOWN_ADR,
    ERROR_INCORRECT_VALUE,
    ERROR_ILEGAL_ADR,
};

enum coil_addresses
{
    ADR_ERROR_ACK,
};

enum reg_addresses
{
    ADR_UT_LEVEL,
    ADR_UT_HEIGTH,
    ADR_UT_GAP,
    ADR_UT_MIN,
    ADR_UT_RESTART,
    ADR_LT_LEVEL,
    ADR_LT_HEIGTH,
    ADR_LT_GAP,
    ADR_LT_MIN,
    ADR_LT_RESTART,
    ADR_PC_STATE,
};

class MemoryHandler
{

public:
    MemoryHandler(PumpController *pumpController);

    int getCoilValue(int address);
    int getRegValue(int address);
    int setCoilValue(int address, bool value);
    int setRegValue(int address, uint16_t value);

private:
    PumpController *pumpCtrl;
};
#endif // memory_handler_h