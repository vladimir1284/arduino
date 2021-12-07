#include <TTVout.h>
#include <fontALL.h>
#include "configs.h"
#include "bar_points.h"
#include "controller.h"
#include "gps.h"
#include "rpm.h"
#include "Arial_round_16x24.h"

#define ORANGE_COLOR  ST7735_YELLOW

class ThermHMI {

 public:
  ThermHMI(TTVout *tft, GPS *gps, RPM *rpm, int *st, Configs *cfgs, ElectroController *controller);

  void  drawHMItemplate(),
        update(),
        showAll();

 private:
  TTVout  *_tft;
  GPS  *_gps;
  RPM  *_rpm;
  Configs *_cfgs;
  ElectroController *_ctrl;
  void animateFan(int speed),
       blinkBatery(),
       drawFan(),
       drawIcon(),
       showConfigs(),
       drawUnits(),
       drawBar(int percent),
       drawBateryIcon(int color),
       drawMarks(int color),
       drawNeedle(int value, int color),
       updateTemp(int value),
       updateVoltage(float voltage),
       updateSpeed(int speed),
       updateRPM(int value),
       updateStatusLabels(),
       showStatusLabels(),
       printDateTime(TinyGPSDate &d, TinyGPSTime &t),
       updateLabel(bool *previousSatus, bool status,
                           int color, char *label, int x, int y),
       showError(int error_code);

  int getColor(float tempValue);

  unsigned int lastTimeTempUdated,
               lastBateryChanged,
               lastTimeFanChanged;
  int current_value, 
      fan, 
      last_error_shown, 
      lastSpeed,
      lastRPM,
      lastFanSpeed,
      indicator_color;
  int *screenTask;
  bool pendingClear,
        batON,
        dots,
        acON,
        overPressureON,
        alarmON,
        speed1ON,
        speed0ON;
  float lastVoltage;
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

