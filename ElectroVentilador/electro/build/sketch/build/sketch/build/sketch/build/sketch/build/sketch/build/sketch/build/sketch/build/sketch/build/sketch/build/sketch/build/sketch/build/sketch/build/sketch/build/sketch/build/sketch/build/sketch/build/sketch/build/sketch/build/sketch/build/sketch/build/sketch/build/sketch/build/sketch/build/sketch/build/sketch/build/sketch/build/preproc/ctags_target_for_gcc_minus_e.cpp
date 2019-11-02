# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino"
# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino"
/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

 ****************************************************/

# 7 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino" 2
# 8 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino" 2
# 9 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino" 2
# 10 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino" 2

# 12 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/ElectroVentilador/electro/electro.ino" 2

unsigned int lastUpdated = 0;
int speed, temp;

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield


// in which case, set this #define pin to 0!


// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(PA4, PA3, PA2 /* you can also connect this to the Arduino reset*/);

// Input pin for the button

Button btn = Button();

int screenTask = WORK;

// Configurations
Configs cfgs = Configs(&tft, &screenTask, &btn);

// HMI
ThermHMI hmi = ThermHMI(&tft, &cfgs);

// Controller
ElectroController controller = ElectroController();

void setup(void)
{

  // Use this initializer if you're using a 1.8" TFT
  SPI.begin();
  tft.initR(0x2); // initialize a ST7735S chip, black tab

  tft.fillScreen(0x0000);
  tft.setRotation(3);

  // Setups
  btn.init(PA10);
  cfgs.init();

  // Init HMI
  hmi.drawHMItemplate(0xFFFF);
  hmi.update(81,FAN_STOP);

  // Init the controller
  controller.init(PA0, PB15, PA9, PA8,
                  cfgs.getTemp(), cfgs.getHyst(),
                  cfgs.getCalibration());
}

void loop()
{
  btn.run();
  cfgs.run();
  controller.run();
  hmi.update(controller.getTemperature(),controller.getFanSpeed());

  // if (millis()-lastUpdated>3000){
  //   speed = random(0,3);
  //   temp = random(30,110);
  //   lastUpdated = millis();
  // }

}
