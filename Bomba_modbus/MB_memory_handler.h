#include "TankParameters.h"
#include "pump_controller.h"

enum coil_addresses
{
    ADR_PUMP,
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
    MemoryHandler();
    int getCoilValue(int address);
    int getRegValue(int address);
    int setCoilValue(int address, bool value);
    int setRegValue(int address, uint16_t value);
};
