#ifndef configs_h
#define configs_h
#include <Arduino.h>
#include "Adafruit_ST7735.h"
#include <EEPROM.h>
#include "button.h"

// ------------ Configuration---------------------
// Time delay for returning to IDLE state
#define WAITING_TIME 12000 // ms
// Time delay for changing between menu elements
#define CHANGE_TIME 1000 // ms

#define RECT_HEIGHT 16
#define RECT_WIDTH  160
// -------------------------------------------------

// Base address for storing configs
#define BASE_ADDRESS 0

// Delay ms for 
#define INIT_BUZZ_DELAY 150
#define INIT_SCREEN_DELAY 3000
#define BUZZER_ON_DELAY 200
#define BUZZER_OFF_DELAY 800
#define BUZZ_FREQ 2000 // Hz

// Validation values for parameters
#define BASE_SHIFT 100
#define MIN_CAL_VAL -5
#define MAX_CAL_VAL 5
#define MIN_TEMP_VAL 75
#define MAX_TEMP_VAL 85 
#define MIN_HYST_VAL 3
#define MAX_HYST_VAL 6

// PINOUT
#define PIN_NTC     PA0
#define PIN_VOLT    PA1
#define PIN_BUZ     PB15
#define PIN_BUZ_ACT PB5
#define PIN_FAN0    PA9
#define PIN_FAN1    PA8
#define BTN_PIN     PA10
#define PIN_A       PA15
#define PIN_B       PB4

// Signals controling directly the fan
#define WAIT2ACTIVATE 250 // ms

// Voltmeter
#define VOLT_MIN 12
#define VOLT_MAX 14.5
#define VOLT_ALARM 14.94
#define MIN_CAL_VOLT -5
#define MAX_CAL_VOLT  5
#define VOLT_FACTOR 0.00459228515625 // 4.7k - 1k


// Simulator
#define SIM_Tmin 70
#define SIM_Tmax 100
#define SIM_Vmin 11
#define SIM_Vmax 16
#define SIM_Vinit 12
#define SIM_COUNT 5
#define SIM_INTERVAL 1000 // ms

// Fan Speeds
enum fan_speeds
{
  FAN_STOP,
  FAN_SPEED_1,
  FAN_SPEED_2
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
  WORK,
  CONFIG,
  TEST,
  PENDING,
  ERROR
};

// Screens
enum screens
{
  DUMMY,
  MAIN,
  TEMP1,
  HYST,
  CALIBRAR,
  VOLTAGE
};

// Max length of configuration strings
#define STRING_LENGTH 14

// Main Screen
#define MAIN_ITEMS 5
PROGMEM const char mainText[MAIN_ITEMS][STRING_LENGTH] = {" Temperatura",
                                                          "  Histeresis",
                                                          "   Calibrar ",
                                                          "   Voltaje  ",
                                                          "    Prueba  "};

// ---------- HMI --------------
// Thermometer icon
#define Xicon 130
#define Yicon 110
#define Rtherm 3
#define Htherm 12
#define Wtherm 3
#define SPACEmarks 4
#define Wmarks 1
#define Lmarks 5
#define SPACEwaves 5
#define Wwaves 4
#define Hwaves 2

// Batery icon
#define Xbat 5
#define Ybat 8
#define Wbat 25
#define Hbat 16
#define Rbat 3
#define TEXTgap 2
#define BatBlinkDelay 500

// Configs
#define Xcfg1 5
#define Ycfg1 70
#define DxCfg1 11
#define DyCfg1 15

// Configs
#define Xlabel  80
#define Ylabel  74
#define DxLabel 18
#define DyLabel 18
#define Ipaso "I"
#define IIpaso "II"
#define AClabel "AC"
#define APlabel "AP"
#define ATlabel "AT"

// Analog indicator
#define MAXangle 90
#define Nmarks 5
#define OUTERradius 100
#define INNERradius 90
#define INNERneedle 60
#define NEEDLEbase 3
#define GAUGEx 105
#define GAUGEy 123
#define MAXindicator 100
#define MINindicator 50

// Temperature refresh interval
#define TEMP_DELAY 500 // ms

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
      getCalibration(),
      getVoltCalibration();

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
      voltCalibration,
      calibration;

  void drawMenu(const char text[][STRING_LENGTH], int nItems),
      leaveMenus(),
      validateConfigs(),
      startTest(),
      startConfig(),
      saveTemp(),
      saveHyst(),
      saveVoltCalibration(),
      saveCalibration(),
      printLargeValue(),
      printLargeVoltValue(),
      printParameterValue(int bg_color),
      printVoltValue(int value),
      printValue(int value),
      prepareEditScreen(const char text[][STRING_LENGTH]),
      setPos(int position);
};
#endif
