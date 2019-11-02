#ifndef configs_h
#define configs_h
#include <Arduino.h>
#include "Adafruit_ST7735.h"
#include <EEPROM.h>
#include "button.h"

// Time delay for returning to IDLE state
#define WAITING_TIME 10000 // ms
// Time delay for changing between menu elements
#define CHANGE_TIME 1000 // ms

// Base address for storing configs
#define BASE_ADDRESS 0

// Validation values for parameters
#define MIN_CAL_VAL -5
#define MAX_CAL_VAL 5
#define MIN_TEMP_VAL 65
#define MAX_TEMP_VAL 85
#define MIN_HYST_VAL 3
#define MAX_HYST_VAL 6

// PINOUT
#define PIN_NTC  PA0
#define PIN_BUZ  PB15
#define PIN_FAN0 PA9
#define PIN_FAN1 PA8

// Fan Speeds
enum fan_speeds
{
  FAN_STOP, FAN_SPEED_1, FAN_SPEED_2
};

// Dual means 2 speed fan
#define DUAL true
#define DUAL_HYST_FACTOR 2
#define TMAX 105

// Sensor error
#define MIN_VAL_CODE 100
#define MAX_VAL_CODE 4000
enum error_codes
{
    NO_ERROR,
    SHORT_CIRCUIT,
    OPEN_CIRCUIT
};

// Screen tasks
enum tasks
{
  WORK, CONFIG, TEST, PENDING, ERROR
};

// Screens
enum screens
{
  DUMMY,
  MAIN,
  TEMP1,
  HYST,
  CALIBRAR
};

// Max length of configuration strings
#define STRING_LENGTH 14

// Main Screen
#define MAIN_ITEMS 4
PROGMEM const char mainText[MAIN_ITEMS][STRING_LENGTH] = {" Temperatura", 
                                                          "  Histeresis",
                                                          "   Calibrar ", 
                                                          "    Prueba  "};

// ---------- HMI --------------
// Thermometer icon
#define Xicon  140
#define Yicon  110
#define Rtherm  3
#define Htherm  12
#define Wtherm  3
#define SPACEmarks  4
#define Wmarks  1
#define Lmarks  5
#define SPACEwaves  5
#define Wwaves  4
#define Hwaves  2

// Analog indicator
#define MAXangle  90
#define Nmarks  5
#define OUTERradius  100
#define INNERradius  90
#define INNERneedle  60
#define NEEDLEbase 3
#define GAUGEx  105
#define GAUGEy  123
#define MAXindicator 110
#define MINindicator 40


// Temperature refresh interval
#define TEMP_DELAY 1000 // ms

// ----------- Fan --------------
#define Xfan 120
#define Yfan 0
#define Wfan 39
#define Hfan 39
#define SLOWdelay 500 // ms
#define FASTdelay 250 // ms

// -----------------------------

class Configs
{

public:
  Configs(Adafruit_ST7735 *tft, int *screenTask, Button *btn);

  void run(); // Main method to be refreshed in every loop
  void init();
  int getTemp(),
      getHyst(),
      getCalibration();

private:
  unsigned int lastTimePressed, lastTimeChanged; // For returning after a while
  Adafruit_ST7735 *_tft;
  int pos,
      value; // Position of the cursor
  String newStr;
  int *screenTask;
  Button *_btn;
  enum screens screen;
  bool configurationNeeded;
  int temp1,
      hysteresis,
      calibration;

  void drawMenu(const char text[][STRING_LENGTH], int nItems),
      leaveMenus(),
      validateConfigs(),
      startTest(),
      startConfig(),
      saveTemp(),
      saveHyst(),
      saveCalibration(),
      printLargeValue(),
      printParameterValue(int bg_color),
      printValue(int value),
      prepareEditScreen(const char text[][STRING_LENGTH]),
      setPos(int position);
};
#endif
