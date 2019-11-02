#include "Adafruit_ST7735.h"
#include "configs.h"

class ThermHMI {

 public:
  ThermHMI(Adafruit_ST7735 *tft, int *st, Configs *cfgs);

  void  drawHMItemplate(int color),
        update(int tempValue, int fanSpeed, int error_code);

 private:
  Adafruit_ST7735  *_tft;
  Configs *_cfgs;
  void animateFan(int speed),
       drawFan(int color),
       drawIcon(int color),
       drawMarks(int color),
       drawNeedle(int value, int color),
       updateTemp(int value, int color),
       showError(int error_code);;

  unsigned int lastTimeFanChanged, lastTimeTempUdated;
  int current_value, fan, last_error_shown;
  int *screenTask;
  bool pendingClear;
};



// Fan BMP in the first position
const uint8_t fan0[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x7c, 0x00, 0x00, 
  0x00, 0x00, 0x3f, 0x80, 0x00, 
  0x00, 0x00, 0x3f, 0xe0, 0x00, 
  0x00, 0x38, 0x1f, 0xe0, 0x00, 
  0x00, 0x7e, 0x1f, 0xe0, 0x00, 
  0x00, 0xff, 0x0f, 0xe0, 0x00, 
  0x01, 0xff, 0x0f, 0xc1, 0x00, 
  0x01, 0xff, 0xff, 0xc3, 0x00, 
  0x03, 0xff, 0xff, 0xc3, 0x80, 
  0x03, 0xff, 0x83, 0xc7, 0x80, 
  0x00, 0x1e, 0x00, 0xef, 0xc0, 
  0x00, 0x0c, 0x7c, 0x7f, 0xc0, 
  0x00, 0x1c, 0xfe, 0x7f, 0xc0, 
  0x00, 0x19, 0xff, 0x3f, 0xe0, 
  0x03, 0xf9, 0xff, 0x3f, 0xe0, 
  0x0f, 0xf9, 0xff, 0x3f, 0xe0, 
  0x0f, 0xf9, 0xff, 0x3f, 0x00, 
  0x0f, 0xf9, 0xff, 0x30, 0x00, 
  0x07, 0xfc, 0xfe, 0x70, 0x00, 
  0x07, 0xfc, 0x7c, 0x60, 0x00, 
  0x07, 0xee, 0x00, 0xf0, 0x00, 
  0x03, 0xc7, 0x83, 0xff, 0x80, 
  0x03, 0x87, 0xff, 0xff, 0x80, 
  0x01, 0x87, 0xff, 0xff, 0x00, 
  0x00, 0x07, 0xe1, 0xff, 0x00, 
  0x00, 0x0f, 0xe1, 0xfe, 0x00, 
  0x00, 0x0f, 0xf0, 0xfc, 0x00, 
  0x00, 0x0f, 0xf0, 0x38, 0x00, 
  0x00, 0x0f, 0xf8, 0x00, 0x00, 
  0x00, 0x03, 0xf8, 0x00, 0x00, 
  0x00, 0x00, 0x7c, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
};
// Fan BMP in the first position
const uint8_t fan1[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x30, 0x00, 0x00, 
  0x00, 0x03, 0xf0, 0x60, 0x00, 
  0x00, 0x0f, 0xf8, 0x70, 0x00, 
  0x00, 0x1f, 0xf8, 0x78, 0x00, 
  0x00, 0x1f, 0xf8, 0x7c, 0x00, 
  0x00, 0x07, 0xf8, 0x7e, 0x00, 
  0x00, 0x01, 0xfc, 0x7f, 0x00, 
  0x00, 0x01, 0xfe, 0x7f, 0x00, 
  0x03, 0xc1, 0xff, 0xff, 0x80, 
  0x07, 0xff, 0x83, 0xff, 0x00, 
  0x07, 0xff, 0x00, 0xfe, 0x00, 
  0x07, 0xfc, 0x7c, 0x7c, 0x00, 
  0x07, 0xfc, 0xfe, 0x70, 0x00, 
  0x07, 0xf9, 0xff, 0x30, 0x00, 
  0x07, 0xf9, 0xff, 0x30, 0x60, 
  0x07, 0x99, 0xff, 0x33, 0xe0, 
  0x06, 0x19, 0xff, 0x3f, 0xe0, 
  0x00, 0x19, 0xff, 0x3f, 0xc0, 
  0x00, 0x1c, 0xfe, 0x3f, 0xc0, 
  0x00, 0x3e, 0x7c, 0x7f, 0xc0, 
  0x00, 0x7e, 0x00, 0xff, 0xc0, 
  0x00, 0xff, 0x83, 0xff, 0xc0, 
  0x01, 0xff, 0xff, 0x07, 0x80, 
  0x01, 0xfe, 0x7f, 0x00, 0x00, 
  0x00, 0xfc, 0x3f, 0x80, 0x00, 
  0x00, 0x7c, 0x3f, 0xc0, 0x00, 
  0x00, 0x3c, 0x3f, 0xf0, 0x00, 
  0x00, 0x1c, 0x3f, 0xf0, 0x00, 
  0x00, 0x0c, 0x1f, 0xe0, 0x00, 
  0x00, 0x00, 0x1f, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
};

