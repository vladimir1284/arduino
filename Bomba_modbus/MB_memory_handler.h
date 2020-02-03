#ifndef memory_handler_h
#define memory_handler_h

#include "TankParameters.h"
#include "pump_controller.h"
#include "configs.h"

#define REGISTER_SIZE 16 // Depends on reg_addresses

enum error_codes
{
    OK_ACTION,
    ERROR_UNKNOWN_ADR,
    ERROR_INCORRECT_VALUE,
    ERROR_ILEGAL_ADR,
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