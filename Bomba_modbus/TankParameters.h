#ifndef TankParameters_h
#define TankParameters_h

#include "configs.h"

class TankParameters
{

public:
  TankParameters();

  void init(int addrHeight, int addrGap, int addrRestart, int addrMin);

  // Getters
  int getHeight();
  int getGap();
  int getRestart();
  int getMin();

  // Setters
  int setHeight(int newValue);
  int setGap(int newValue);
  int setRestart(int newValue);
  int setMin(int newValue);

  // Must be protected in order to have multiple instances!!!!
private:
  int height,  // Distance from the sensor to the bottom (cm)
      gap,     // Distance from the sensor to the maximun level of water (cm)
      restart, // Percent of the capacity for restart pumping
      min,     // Minimum percent of the capacity that can be accepted
      addr_height,
      addr_gap,
      addr_restart,
      addr_min;
};
#endif