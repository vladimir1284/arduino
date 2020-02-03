#include "TankParameters.h"
/*
  Class for handling tank parameters.
*/
// Constructor
TankParameters::TankParameters()
{
}

void TankParameters::init(int addrHeight, int addrGap, int addrRestart, int addrMin)
{
  addr_height = addrHeight;
  addr_gap = addrGap;
  addr_restart = addrRestart;
  addr_min = addrMin;
  height = loadInt(addr_height);
  gap = loadInt(addr_gap);
  min = loadInt(addr_restart);
  restart = loadInt(addr_min);

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
    saveInt(addr_height, newValue);
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
    saveInt(addr_gap, newValue);
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
    saveInt(addr_restart, newValue);
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
    saveInt(addr_min, newValue);
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
  return height;
}

//----------------------------------------------------------------------------------
int TankParameters::getGap()
{
  return gap;
}

//----------------------------------------------------------------------------------
int TankParameters::getMin()
{
  return min;
}

//----------------------------------------------------------------------------------
int TankParameters::getRestart()
{
  return restart;
}
