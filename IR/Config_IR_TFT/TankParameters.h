#include <EEPROM.h>


// Maximum height for a tank
#define MAX_HEIGHT  300 // cm
// Minimum gap for a tank
#define MIN_GAP  10 // cm

class TankParameters {

  public:
    TankParameters(uint16 baseAddress = 0);

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
  protected:
    uint16 height; // Distance from the sensor to the bottom (cm)
    uint16 gap;    // Distance from the sensor to the maximun level of water (cm)
    uint16 restart;// Percent of the capacity for restart pumping
    uint16 min;    // Minimum percent of the capacity that can be accepted
    uint16 baseAddress; // Base address in flash

};
