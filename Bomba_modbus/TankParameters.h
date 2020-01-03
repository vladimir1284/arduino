#ifndef TankParameters_h
#define TankParameters_h

#include <EEPROM.h>
// Base addresses in flash
#define BASE_ADR_UPPER_TANK 0x00
#define BASE_ADR_LOWER_TANK 0x20


// Maximum height for a tank
#define MAX_HEIGHT  300 // cm
// Minimum gap for a tank
#define MIN_GAP  10 // cm

class TankParameters {

  public:
    TankParameters();

    void init(int baseAdr);

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
    uint16 height; // Distance from the sensor to the bottom (cm)
    uint16 gap;    // Distance from the sensor to the maximun level of water (cm)
    uint16 restart;// Percent of the capacity for restart pumping
    uint16 min;    // Minimum percent of the capacity that can be accepted
    uint16 baseAddress; // Base address in flash

};
#endif