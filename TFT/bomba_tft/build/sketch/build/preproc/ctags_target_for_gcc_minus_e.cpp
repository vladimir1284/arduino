# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino"
# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino"
/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

 ****************************************************/

# 7 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino" 2
# 8 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino" 2
# 9 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino" 2
# 10 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino" 2
# 11 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino" 2
# 12 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/TFT/bomba_tft/bomba_tft.ino" 2

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
PumpController pump = PumpController();

PumpHMI hmi = PumpHMI(&tft);

// For simulation
int levelUp;
int levelDown;

int lastPrint = 0;


void setup(void)
{

  // Use this initializer if you're using a 1.8" TFT
  SPI.begin();
  tft.initR(0x2); // initialize a ST7735S chip, black tab

  tft.fillScreen(0x0000);
  tft.setRotation(0);

  // Try text
  tft.setTextSize(1);
  tft.setTextColor(0xFFFF, 0x0000);

  // Setup HMI configurations
  hmi.drawHMItemplate(pump.getLowerTankMin(), pump.getUpperTankMin());

  // Setup IR configurations
  configurator.init(pump.getUpperTank(),pump.getLowerTank());

  // Setup Pump Controller configurations
  pump.init();

}

void loop()
{
  int level, count = 0;



  if ((millis() - lastPrint) > 1000)
  {
    //simulate();
    //hmi.animate();
    hmi.updateLowerTankLevel(pump.getLowerTankLevel());
    hmi.updateUpperTankLevel(pump.getUpperTankLevel());

    // // Print the distance
    // tft.setCursor(70, 40);
    // char tbs[6];
    // sprintf(tbs, "%3dmm", level);
    // tft.println(tbs);
    // lastPrint = millis();
  }
}

// void simulate()
// {
//   hmi.updateLowerTankLevel(levelDown);
//   hmi.updateUpperTankLevel(levelUp);
//   levelUp = (levelUp + 5) % 100;
//   levelDown = 100 - levelUp;
// }
