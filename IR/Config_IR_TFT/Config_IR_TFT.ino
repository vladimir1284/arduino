/*
 * irpm test for stm32f103cb generic blue board under stm32duino
 * Ir pin entry is PC15
 * jp Cocatrix 2017
 */
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "configs_ir.h"

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     PA4
#define TFT_RST    PA2  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     PA3

// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

bool LOCK_SCREEN = false;

IRConfigs configurator = IRConfigs(&tft, &LOCK_SCREEN);

void setup() {
    // put your setup code here, to run once:
   // Use this initializer if you're using a 1.8" TFT
    SPI.begin();
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

    tft.fillScreen(ST7735_BLACK);
    tft.setRotation(0);

  // Try text
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

    configurator.init();
}

void loop() {
    configurator.run();
}

// #include "TankParameters.h"
// // Tanks
// int height1, height2;

// void setup()
// {
//   TankParameters lowerTank = TankParameters(0);
//   TankParameters upperTank = TankParameters(20);
//   //lowerTank.setHeight(15);
//   //upperTank.setHeight(25);
//   height1 = lowerTank.getHeight();
//   height2 = upperTank.getHeight();
//   delay(1000);
// }

// void loop()
// {
// }