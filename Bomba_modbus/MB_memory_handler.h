#ifndef memory_handler_h
#define memory_handler_h

#include "TankParameters.h"
#include "pump_controller.h"
#include "LightController.h"
#include "configs.h"

#define REGISTER_SIZE 64 // Depends on reg_addresses

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
    MemoryHandler(PumpController *pumpController,
                  LightController *lamp0);

    int getCoilValue(int address);
    int getRegValue(int address);
    int setCoilValue(int address, bool value);
    int setRegValue(int address, uint16_t value);

private:
    PumpController *pumpCtrl;
    LightController *light0;
};
#endif // memory_handler_h