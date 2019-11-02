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
#define MIN_CAL_VAL -10
#define MAX_CAL_VAL 10
#define MIN_TEMP_VAL 65
#define MAX_TEMP_VAL 85
#define MIN_HYST_VAL 3
#define MAX_HYST_VAL 6


// Screen tasks
enum tasks
{
  WORK, CONFIG, TEST
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
