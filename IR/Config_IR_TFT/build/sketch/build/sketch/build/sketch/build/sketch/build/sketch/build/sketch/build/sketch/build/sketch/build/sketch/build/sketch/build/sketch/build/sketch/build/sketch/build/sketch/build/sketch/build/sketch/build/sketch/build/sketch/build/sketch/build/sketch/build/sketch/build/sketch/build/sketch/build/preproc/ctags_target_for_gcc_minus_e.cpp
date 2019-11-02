# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/IR/Config_IR_TFT/Config_IR_TFT.ino"
# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/IR/Config_IR_TFT/Config_IR_TFT.ino"
/*
 * irpm test for stm32f103cb generic blue board under stm32duino
 * Ir pin entry is PC15
 * jp Cocatrix 2017
 */
# 7 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/IR/Config_IR_TFT/Config_IR_TFT.ino" 2
# 8 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/IR/Config_IR_TFT/Config_IR_TFT.ino" 2
# 9 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/IR/Config_IR_TFT/Config_IR_TFT.ino" 2
# 10 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/IR/Config_IR_TFT/Config_IR_TFT.ino" 2

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield


// in which case, set this #define pin to 0!


// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(PA4, PA3, PA2 /* you can also connect this to the Arduino reset*/);

bool LOCK_SCREEN = 0x0;

IRConfigs configurator = IRConfigs(&tft, &LOCK_SCREEN);

void setup() {
    // put your setup code here, to run once:
   // Use this initializer if you're using a 1.8" TFT
    SPI.begin();
    tft.initR(0x2); // initialize a ST7735S chip, black tab

    tft.fillScreen(0x0000);
    tft.setRotation(0);

  // Try text
    tft.setTextSize(1);
    tft.setTextColor(0xFFFF, 0x0000);

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
