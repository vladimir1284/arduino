#include "configs.h"

int saveInt(int address, int value)
{
    EEPROM.write(EEPROM.PageBase0 + address * 4, (uint16)value);
}

int loadInt(int address)
{
    uint16 x;
    EEPROM.read(EEPROM.PageBase0 + 4 * address, &x);
    return x;
}