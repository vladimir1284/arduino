/*
STM32 adaption by Matthias Diro, tested with maple mini and heltec OLED 12864 I2c; adress: 0x3C (SPI should work, but I didn't own one)
Things to know:
 This adaption uses hardware I2C (now Wire.h), Port: I2c2. SDA=0, SCL=1 on maple mini
 further details: STM32_README.txt
*/

#include "TrueRMS.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>
#include <EEPROM.h>

/* Useful Constants */
#define SECS_PER_MIN (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY (SECS_PER_HOUR * 24L)

/* Useful Macros for getting elapsed time */
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY) // this is number of days since Jan 1 1970

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#define SETUPTIME 5000 //ms

#define SAMPLE_INTERVAL 150 // us
#define NSAMPLES 128
#define NAVERAGE 50

#define VRange 1720
#define voltage_pin PA1 // us
#define MINVOLTAGE 80   // Thresshold for detecting power off

#define IRange 30.2
#define current_pin PA0 // us

#define EFACTOR 2.778e-10

Power acPower;

unsigned long lastEnergyMS, startup;
int averageIter;
float Vaverage, Iaverage, Paverage, FPaverage;
float energy;

bool pendingSaving; // Enables power off saving 5s after power on.
                    // It's disabled after saving and restores after 1hr.

#define SCR_ALT_DELAY 5000
bool showTimeOn; // Used for alternating betwing showing time on and power.
unsigned long lastScreenAlternate;

void setup()
{
  uint16 savedEnergy1, savedEnergy2;

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  display.setTextSize(2);
  display.setTextColor(WHITE);

  acPower.begin(VRange, IRange, NSAMPLES, ADC_12BIT, BLR_ON, CNT_SCAN);
  acPower.start();

  // EEPROM
  EEPROM.PageBase0 = 0x801F000;
  //EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize = 0x400;
  // Read initial values from memory
  EEPROM.read(EEPROM.PageBase0, &savedEnergy1);
  EEPROM.read(EEPROM.PageBase0 + 1, &savedEnergy2);

  energy = ((uint32)savedEnergy1 | (((uint32)savedEnergy2 << 16) & 0xFFFF0000)) / 10000.0;

  // Show stored values on power on
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(savedEnergy1);
  display.setCursor(0, 24);
  display.print(savedEnergy2);
  display.display();
  delay(2000);

  averageIter = 0;
  lastEnergyMS = millis();

  // Prepare saving
  startup = millis();
  pendingSaving = false;

  // Alternating screen
  lastScreenAlternate = millis();
  showTimeOn = false;
}

void loop()
{
  computeAverage();
  saveIfNeeded();
  handleOverflow();
}

// Generates the power on time string
void getTimeStr(char *str)
{
  int val = millis() / 1000;
  int days = elapsedDays(val);
  int hours = numberOfHours(val);
  int minutes = numberOfMinutes(val);

  // digital clock display of current time
  sprintf(str, "%03d:%02d:%02d", days, hours, minutes);
}

// Produces overflow after 10000kwh
void handleOverflow()
{
  if (energy > 10000)
  {
    energy = 0;
  }
}

// Save energy counter on power off
void saveIfNeeded()
{
  // After a setup time, enable the saving on power off
  // The time delay is needed to allow voltage readings to stabilize
  if ((millis() - startup > SETUPTIME) && (startup != 0))
  {
    pendingSaving = true;
    startup = 0;
  }
  // Checkout voltage if save is enabled
  if (pendingSaving)
  {
    if (acPower.rmsVal1 < MINVOLTAGE) // Power off detected
    {
      // Save energy counter multiply by 10000 using two SHORT (16bits) values;
      int energia = (int)(energy * 10000.0);
      uint16 low = (uint16)(energia & 0x0000FFFF);
      EEPROM.write(EEPROM.PageBase0, low);
      uint16 high = (uint16)(energia >> 16) & 0x0000FFFF;
      EEPROM.write(EEPROM.PageBase0 + 1, high);
      pendingSaving = false;
    }
  }
}

// computes average values of electric parameters
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
  Paverage += abs(acPower.realPwr);
  FPaverage += acPower.pf;
  averageIter++;

  if (averageIter == NAVERAGE)
  {
    Vaverage /= NAVERAGE;
    Iaverage /= NAVERAGE;
    p = (int)Paverage / NAVERAGE;
    FPaverage /= NAVERAGE;
    // Update Energy Counter
    energy += EFACTOR * (millis() - lastEnergyMS) * p;
    lastEnergyMS = millis();
    // Update Screen
    updateScreen(Vaverage, Iaverage, p, FPaverage);
    averageIter = 0;
  }
}

// Reads the analog instantaneus values corresponding to V and I
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

// Updates data in oled screen
void updateScreen(float v, float i, int p, float fp)
{
  char tbs[5];
  char energyStr[12];
  char timeStr[9];

  display.clearDisplay();

  // First line
  display.setCursor(0, 0);
  display.print(v, 0);
  display.print("V");

  display.setCursor(64, 0);
  display.print(floor(i * 10) / 10, 1);
  display.print("A");

  // Second line
  display.setCursor(0, 24);

  if (showTimeOn)
  {
    getTimeStr(timeStr);
    display.print(timeStr);
  }
  else
  {
    display.print(p);
    display.print("w ");

    display.setCursor(64, 24);
    sprintf(tbs, "fp.%02d", (int)abs(100 * fp));
    display.print(tbs);
  }

  // Alternate screen
  if (millis() - lastScreenAlternate > SCR_ALT_DELAY){
    lastScreenAlternate = millis();
    showTimeOn = !showTimeOn;
  }

  // Third line
  display.setCursor(0, 48);
  if (energy < 1)
  {
    sprintf(energyStr, "%06.2fWh", 1000 * energy);
  }
  else
  {
    sprintf(energyStr, "%06.2fkWh", energy);
  }
  display.print(energyStr);

  display.display();
}
