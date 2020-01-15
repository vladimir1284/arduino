
#include "Adafruit_ST7735.h"
#include "configs.h"

// For animation
PROGMEM const static int16_t  Xup[] = {p3x-d/2+1,p3x-d/2+1,p1x+b1+1};
PROGMEM const static int16_t  Xdown[] = {p3x-dout-din/2+1,p3x-dout-din/2+1,p3x-dout-din/2+d+1};
PROGMEM const static int16_t  Yup[] = {p3y-dout/2-d,p1y-h1+5*d/2,p1y-h1+d+1};
PROGMEM const static int16_t  Ydown[] = {p2y-h2-d,p3y+d,p3y-d/2+1};

class PumpHMI {

 public:
  PumpHMI(Adafruit_ST7735 *tft);

  void  drawHMItemplate(int16_t lowerMin, int16_t upperMin),
        updateUpperTankLevel(int16_t percent),
        animate(),
        updateLowerTankLevel(int16_t percent);

 private:
  Adafruit_ST7735  *_tft;
  int16_t _lowerMin,
          _upperMin,
          _animationStage;
};
