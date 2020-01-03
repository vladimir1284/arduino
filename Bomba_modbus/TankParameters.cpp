#include "TankParameters.h"
/*
  Class for handling tank parameters.
*/
// Constructor
TankParameters::TankParameters()
{
}

void TankParameters::init(int baseAdr){
  uint16 h, g, m, r;

  // Configure Memory
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize = 0x400;

  // Read from EEPROM
  baseAddress = baseAdr;
  EEPROM.read(EEPROM.PageBase0 + baseAddress, &h);
  EEPROM.read(EEPROM.PageBase0 + baseAddress + 4, &g);
  EEPROM.read(EEPROM.PageBase0 + baseAddress + 8, &m);
  EEPROM.read(EEPROM.PageBase0 + baseAddress + 12, &r);

  height = h;
  gap = g;
  min = m;
  restart = r;

  // Verify and Auto-correct
  if (height < MIN_GAP || height > MAX_HEIGHT)
  {
    setHeight(MAX_HEIGHT - 1);
  }
  if (gap < MIN_GAP || gap > MAX_HEIGHT)
  {
    setGap(MIN_GAP);
  }
  if (min < 0 || min > MAX_HEIGHT)
  {
    setMin(20);
  }
  if (restart < 0 || restart > MAX_HEIGHT)
  {
    setRestart(50);
  }
}

//----------------------------------------------------------------------------------
int TankParameters::setHeight(int newValue)
{
  if (newValue > MIN_GAP && newValue < MAX_HEIGHT)
  {
    height = newValue;
    EEPROM.write(EEPROM.PageBase0 + baseAddress, (uint16)height);
    return 0;
  }
  else
  {
    return -1;
  }
}

//----------------------------------------------------------------------------------
int TankParameters::setGap(int newValue)
{
  if (newValue >= MIN_GAP &&
      newValue < height)
  {
    gap = newValue;
    EEPROM.write(EEPROM.PageBase0 + baseAddress + 4, (uint16)gap);
    return 0;
  }
  else
  {
    return -1;
  }
}

//----------------------------------------------------------------------------------
int TankParameters::setRestart(int newValue)
{
  if (newValue > 0 && newValue < 100)
  {
    restart = newValue;
    EEPROM.write(EEPROM.PageBase0 + baseAddress + 12, (uint16)restart);
    return 0;
  }
  else
  {
    return -1;
  }
}

//----------------------------------------------------------------------------------
int TankParameters::setMin(int newValue)
{
  if (newValue > 0 && newValue < 100)
  {
    min = newValue;
    EEPROM.write(EEPROM.PageBase0 + baseAddress + 8, (uint16)min);
    return 0;
  }
  else
  {
    return -1;
  }
}

//----------------------------------------------------------------------------------
int TankParameters::getHeight()
{
  //EEPROM.read(EEPROM.PageBase0+baseAddress, &height);
  return height;
}

//----------------------------------------------------------------------------------
int TankParameters::getGap()
{
  //EEPROM.read(EEPROM.PageBase0+baseAddress+4, &gap);
  return gap;
}

//----------------------------------------------------------------------------------
int TankParameters::getMin()
{
  //EEPROM.read(EEPROM.PageBase0+baseAddress+8, &min);
  return min;
}

//----------------------------------------------------------------------------------
int TankParameters::getRestart()
{
  //EEPROM.read(EEPROM.PageBase0+baseAddress+12, &restart);
  return restart;
}
