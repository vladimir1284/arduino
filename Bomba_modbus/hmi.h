
#include "Adafruit_ST7735.h"
#include "configs.h"


#define b2 64
#define b1 b2 / 2
#define h1 50
#define c1 b1 / 2
#define h2 40
#define d 8
#define p1x 2
#define p1y 160 - 100
#define p2x 2
#define p2y 160 - 2
#define p3x 48
#define p3y 160 - 81
#define din 16
#define dout 32
#define h0 160 - 8

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
