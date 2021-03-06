/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "hmi.h"
#include "configs.h"
#include "controller.h"

unsigned int lastUpdated = 0;
int speed, temp;

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS PA4
#define TFT_RST PA2 // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC PA3

// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Input pin for the button
Button btn = Button();

int screenTask = WORK;

// Simulator
bool testPrepared = false;

// Controller
ElectroController controller = ElectroController();

// Configurations
Configs cfgs = Configs(&tft, &screenTask, &btn);

// HMI
ThermHMI hmi = ThermHMI(&tft, &screenTask, &cfgs, &controller);

void setup(void)
{

  // Use this initializer if you're using a 1.8" TFT
  SPI.begin();
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab

  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);

  screenTask = WORK;

  // Setups
  btn.init(BTN_PIN);
  cfgs.init();

  // Init the controller
  controller.init(PIN_NTC, PIN_BUZ, PIN_FAN0, PIN_FAN1,
                  cfgs.getTemp(), cfgs.getHyst(),
                  cfgs.getCalibration(),cfgs.getVoltCalibration(),
                  PIN_A, PIN_B, PIN_VOLT, PIN_BUZ_ACT);

  // Buzzer sound indicating ready
  int i;
  for (i = 0; i < 2; i++)
  {
    controller.turnOnBuzzer();
    delay(INIT_BUZZ_DELAY);
    controller.turnOffBuzzer();
    delay(INIT_BUZZ_DELAY / 2);
  }
  // Init screen
    if (screenTask == WORK)
  {
    hmi.drawHMItemplate();
    hmi.showAll();
    delay(INIT_SCREEN_DELAY);
  }
}

void loop()
{
  btn.run();
  cfgs.run();
  if (screenTask != TEST)
  {
    controller.run();
    hmi.update();
  }
  else
  {
    controller.simulate();
    hmi.update();
  }
  verifyConfigChanges();
}

void verifyConfigChanges()
{
  if (screenTask == PENDING)
  {
    controller.setTemp1(cfgs.getTemp());
    controller.setHysteresis(cfgs.getHyst());
    controller.setCalibration(cfgs.getCalibration());
    controller.setVoltCalibration(cfgs.getVoltCalibration());

    hmi.drawHMItemplate();

    screenTask = WORK;
  }
  if (screenTask == TEST && !testPrepared)
  {
    controller.setTemp1(cfgs.getTemp());
    controller.setHysteresis(cfgs.getHyst());
    controller.setCalibration(cfgs.getCalibration());
    controller.setVoltCalibration(cfgs.getVoltCalibration());

    hmi.drawHMItemplate();

    testPrepared = true;
  }
}
