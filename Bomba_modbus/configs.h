#ifndef config_h
#define config_h

#include <EEPROM.h>

// ---------- Controller ------------
// Pinout
// Ultrasonic sensors configs
#define triggerPIN PB15
#define N_SENSORS 2
// Pump pins
#define pumpPIN PB8
#define capacitorPIN PB9
// Light pins
#define pirPIN PB5
#define ldrPIN PA1
#define LightPIN0 PB7

// Tanks
#define UPPERTANK 0
#define LOWERTANK 1

// Maximum height for a tank
#define MAX_HEIGHT 300 // cm
// Minimum gap for a tank
#define MIN_GAP 15 // cm

// Control
#define START_CAP_FACTOR 1000 // seg
#define PUMP_ON_FACTOR 60000  // min

// ----------- Modbus ---------------
#define MB_SPEED 19200

// ----------- HMI ------------------
// Pinout
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS PA4
#define TFT_RST PA2 // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC PA3

// ----------- ADDRESSES ------------------

enum addresses
{
    //  ----------- Configs-----------
    // Pump Controller 0
    ADR_UT_HEIGTH,
    ADR_UT_GAP,
    ADR_UT_MIN,
    ADR_UT_RESTART,
    ADR_LT_HEIGTH,
    ADR_LT_GAP,
    ADR_LT_MIN,
    ADR_LT_RESTART,
    ADR_PUMP_START_CAP,
    ADR_PUMP_ON,
    ADR_FULL_TANK,
    // Light 0
    ADR_LIGHT_MODE_0,
    ADR_LIGHT_SLEEP_TIME_0,
    ADR_LIGHT_SMART_0,
    ADR_LIGHT_SMART_DELAY_0,
    ADR_LIGHT_INIT_DELAY_0,
    ADR_LIGHT_DELAY_INCREMENT_0,
    ADR_LIGHT_TRESHOLD_0,
    //  ----------- Vaiables-----------
    // Pump Controller 0
    ADR_UT_LEVEL,
    ADR_LT_LEVEL,
    ADR_PC_STATE,
    ADR_ERROR_ACK,
    // Light 0
    ADR_LUMINOSITY_0,
    ADR_PIR_STATE_0,
    ADR_LIGHT_STATE_0,
};

// EEPROM utility functions
int saveInt(int address, int value);
int loadInt(int address);

#endif // !config_h