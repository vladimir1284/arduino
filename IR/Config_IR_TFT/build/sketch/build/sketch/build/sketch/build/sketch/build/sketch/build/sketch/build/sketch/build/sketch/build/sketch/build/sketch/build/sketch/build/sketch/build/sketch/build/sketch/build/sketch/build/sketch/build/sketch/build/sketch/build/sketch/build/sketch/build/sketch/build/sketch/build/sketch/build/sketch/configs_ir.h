#include <Arduino.h>
#include "Adafruit_ST7735.h"
#include <irmp.h>
#include "TankParameters.h"

// Buttons for NEC IR protocol
#define POWER_BTN 10
#define LEFT_BTN  19
#define RIGHT_BTN 20
#define OK_BTN    21
#define UP_BTN    17
#define DOWN_BTN  18

// Time delay for returning to IDLE state
#define WAITING_TIME  30000 // ms

// Max number of characters in a parameter value
#define MAX_INPUT_LENGTH 3

// Screens
enum screens {DUMMY, MAIN, PUMP, LIGHT, PUMP_CFG, TANK_CFG, EDIT};

// Max length of configuration strings
#define STRING_LENGTH 12

// Base addresses in flash
#define BASE_ADR_UPPER_TANK 0x00
#define BASE_ADR_LOWER_TANK 0x30

// Main Screen
#define MAIN_ITEMS 2
PROGMEM const char mainText[MAIN_ITEMS][STRING_LENGTH]  = {"Bomba", "Luz"};
// Pump Screen
#define PUMP_ITEMS 2
PROGMEM const char pumpText[PUMP_ITEMS][STRING_LENGTH]  = {"Llenar","Configurar"};
// Pump Config Screen
#define PUMP_CFG_ITEMS 2
PROGMEM const char pumpCfgText[PUMP_CFG_ITEMS][STRING_LENGTH]  = {"Tanque", "Cisterna"};
// Pump Config Screen
#define TANK_CFG_ITEMS 4
PROGMEM const char tankCfgText[TANK_CFG_ITEMS][STRING_LENGTH]  = {"Altura",
                                                                "Brecha",
                                                                "Reinic",
                                                                "Minimo"};



class IRConfigs {

  public:
    IRConfigs(Adafruit_ST7735 *tft, bool *LOCK_SCREEN);

    void run(); // Main method to be refreshed in every loop
    void init(); 
    
    

  private:
    unsigned int lastTimePressed; // For returning after a while
    Adafruit_ST7735 *_tft;
    int pos; // Position of the cursor
    String newStr;
    bool *lockScreen;
    IRMP_DATA irmp_data;
    enum screens screen;
    TankParameters upperTank,
                  lowerTank;
    TankParameters *currentTank;

    void  drawMenu(const char text[][STRING_LENGTH], int nItems),
          leaveMenus(),
          drawTankParameters(),
          printParameterValue(int bg_color),
          prepareEditScreen(),
          setPos(const char text[][STRING_LENGTH], int nItems, int position);


    // IR setup
    void timer2_init ();
    static void TIM2_IRQHandler();

};
