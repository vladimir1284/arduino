/*
STM32 adaption by Matthias Diro, tested with maple mini and heltec OLED 12864 I2c; adress: 0x3C (SPI should work, but I didn't own one)
Things to know:
 This adaption uses hardware I2C (now Wire.h), Port: I2c2. SDA=0, SCL=1 on maple mini
 further details: STM32_README.txt
*/
/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include "TrueRMS.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#define SAMPLE_INTERVAL 150 // us
#define NSAMPLES 128
#define NAVERAGE 50

#define VRange 1720
#define voltage_pin PA1 // us

#define IRange 30.2
#define current_pin PA0 // us

#define EFACTOR 2.778e-8 //2.778e-10

Power acPower;

unsigned long lastEnergyMS;
int averageIter;
float Vaverage, Iaverage, Paverage, FPaverage, energy;

void setup()
{

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  display.setTextSize(2);
  display.setTextColor(WHITE);

  acPower.begin(VRange, IRange, NSAMPLES, ADC_12BIT, BLR_ON, CNT_SCAN);
  acPower.start();

  averageIter = 0;
  energy = 0;
  lastEnergyMS = millis();
}

void loop()
{
  computeAverage();
}
void computeAverage()
{
  int p; 

  if (averageIter == 0)
  {
    Vaverage = 0;
    Iaverage = 0;
    Paverage = 0;
    FPaverage = 0;
  }

  readSamples();
  acPower.publish();
  Vaverage += acPower.rmsVal1;
  Iaverage += acPower.rmsVal2;
  Paverage += acPower.realPwr;
  FPaverage += acPower.pf;
  averageIter++;

  if (averageIter == NAVERAGE)
  {
    Vaverage /= NAVERAGE;
    Iaverage /= NAVERAGE;
    p = (int)Paverage/NAVERAGE;
    FPaverage /= NAVERAGE;
    // Update Energy Counter
    energy += EFACTOR*(millis()-lastEnergyMS)*p;
    lastEnergyMS = millis();
    // Update Screen
    updateScreen(Vaverage, Iaverage, p, FPaverage, energy);
    averageIter = 0;
  }
}

void readSamples()
{
  int i;
  for (i = 0; i < NSAMPLES; i++)
  {
    int V = analogRead(voltage_pin);
    V = analogRead(voltage_pin);
    int I = analogRead(current_pin);
    I = analogRead(current_pin);
    acPower.update(V, I);
    delay_us(SAMPLE_INTERVAL);
  }
}

void updateScreen(float v, float i, int p, float fp, float e)
{
  char tbs[5];
  char energyStr[12];

  display.clearDisplay();

  display.setCursor(0, 0);
  display.print(v, 0);
  display.print("V");

  display.setCursor(64, 0);
  display.print(floor(i * 10) / 10, 1);
  display.print("A");

  display.setCursor(0, 24);
  display.print(p);
  display.print("w ");

  display.setCursor(64, 24);
  sprintf(tbs, "fp.%02d", (int)abs(100 * fp));
  display.print(tbs);

  display.setCursor(0, 48);
  sprintf(energyStr, "%06.2fkWh", e);
  display.print(energyStr);
  //display.print("kWh");

  display.display();
}
