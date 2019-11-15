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
unsigned int lastTimeUpdated = 0;
int simulatedTemperature = SIM_Tmin - 1;
bool simulationAscend = true;
bool testPrepared = false;

// Configurations
Configs cfgs = Configs(&tft, &screenTask, &btn);

// HMI
ThermHMI hmi = ThermHMI(&tft, &screenTask, &cfgs);

// Controller
ElectroController controller = ElectroController();

void setup(void)
{

  // Use this initializer if you're using a 1.8" TFT
  SPI.begin();
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab

  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);

  // Setups
  btn.init(BTN_PIN);
  cfgs.init();

  // Init HMI
  if (screenTask == WORK)
  {
    hmi.drawHMItemplate(ST7735_WHITE);
  }

  // Init the controller
  controller.init(PIN_NTC, PIN_BUZ, PIN_FAN0, PIN_FAN1,
                  cfgs.getTemp(), cfgs.getHyst(),
                  cfgs.getCalibration(), PIN_A, PIN_B);

  // Buzzer sound indicating ready
  int i;
  for (i = 0; i < 2; i++)
  {
    controller.turnOnBuzzer();
    delay(INIT_BUZZ_DELAY);
    controller.turnOffBuzzer();
    delay(INIT_BUZZ_DELAY / 2);
  }
}

void loop()
{
  btn.run();
  cfgs.run();
  if (screenTask != TEST)
  {
    controller.run();
    hmi.update(controller.getTemperature(),
               controller.getFanSpeed(),
               controller.getErrorCode());
  }
  else
  {
    controller.run(simulatorGetTemp());
    hmi.update(simulatorGetTemp(),
               controller.getFanSpeed(),
               controller.getErrorCode());
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

    hmi.drawHMItemplate(ST7735_WHITE);

    screenTask = WORK;
  }
  if (screenTask == TEST && !testPrepared)
  {
    controller.setTemp1(cfgs.getTemp());
    controller.setHysteresis(cfgs.getHyst());
    controller.setCalibration(cfgs.getCalibration());

    hmi.drawHMItemplate(ST7735_WHITE);

    testPrepared = true;
  }
}

int simulatorGetTemp()
{
  if (millis() - lastTimeUpdated > SIM_INTERVAL)
  {
    if (simulationAscend)
    {
      simulatedTemperature++;
      if (simulatedTemperature >= SIM_Tmax)
      {
        simulationAscend = false;
      }
    }
    else
    {
      simulatedTemperature--;
      if (simulatedTemperature < SIM_Tmin)
      {
        screenTask = WORK;
        testPrepared = false;
        simulationAscend = true;
      }
    }
    lastTimeUpdated = millis();
  }

  return simulatedTemperature;
}
