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

#define CALIBRATE false // Set to true for calibration screen

// =============== Watchdog ======================
#define LIBMAPPLE_CORE //comment it for HAL based core

#define IWDG_PR_DIV_4 0x0
#define IWDG_PR_DIV_8 0x1
#define IWDG_PR_DIV_16 0x2
#define IWDG_PR_DIV_32 0x3
#define IWDG_PR_DIV_64 0x4
#define IWDG_PR_DIV_128 0x5
#define IWDG_PR_DIV_256 0x6

typedef enum iwdg_prescaler
{
  IWDG_PRE_4 = IWDG_PR_DIV_4,     /**< Divide by 4 */
  IWDG_PRE_8 = IWDG_PR_DIV_8,     /**< Divide by 8 */
  IWDG_PRE_16 = IWDG_PR_DIV_16,   /**< Divide by 16 */
  IWDG_PRE_32 = IWDG_PR_DIV_32,   /**< Divide by 32 */
  IWDG_PRE_64 = IWDG_PR_DIV_64,   /**< Divide by 64 */
  IWDG_PRE_128 = IWDG_PR_DIV_128, /**< Divide by 128 */
  IWDG_PRE_256 = IWDG_PR_DIV_256  /**< Divide by 256 */
} iwdg_prescaler;

#if defined(LIBMAPPLE_CORE)
typedef struct iwdg_reg_map
{
  volatile uint32_t KR;  /**< Key register. */
  volatile uint32_t PR;  /**< Prescaler register. */
  volatile uint32_t RLR; /**< Reload register. */
  volatile uint32_t SR;  /**< Status register */
} iwdg_reg_map;

#define IWDG ((struct iwdg_reg_map *)0x40003000)
#endif

void iwdg_feed(void)
{
  IWDG->KR = 0xAAAA;
}

void iwdg_init(iwdg_prescaler prescaler, uint16_t reload)
{
  IWDG->KR = 0x5555;
  IWDG->PR = prescaler;
  IWDG->RLR = reload;
  IWDG->KR = 0xCCCC;
  IWDG->KR = 0xAAAA;
}
// ========================================================

// =============== Convert millis() to Days:Hours:minutes ======================
/* Useful Constants */
#define SECS_PER_MIN (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY (SECS_PER_HOUR * 24L)

/* Useful Macros for getting elapsed time */
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY) // this is number of days since Jan 1 1970
// ==============================================================================

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

// =================== Computations ==========================
#define SAMPLE_INTERVAL 150 // us. Tieme interval between samples
#define NSAMPLES 128        // Number of samples used for estimating electric values
#define NAVERAGE 50         // Averaged values of the electric values

#define VRange 430.71   // Voltage calibration constant
#define voltage_pin PA1 // Pin for the voltage divider
#define MINVOLTAGE 80   // Thresshold for detecting power off

#define IRange 51.55    // Current calibration constant
#define current_pin PA0 // Pin for the ACS712 sensor

#define EFACTOR 2.778e-10

Power acPower;
unsigned long lastEnergyMS, startup;
int averageIter;
float Vaverage, Iaverage, Paverage, FPaverage;
float energy;
// ===========================================================

// =============== Auto save ======================
/*  Enables power off saving 5s after power on.
    It's disabled after saving and restores after 1hr.*/
#define ADDR 2 // storage address

bool pendingSaving;
#define SETUPTIME 5000 // ms

#define AUTOSAVEDELAY 3600000 // ms. 1 Hour.
unsigned long lastSave;
// ================================================

// Alternating betwing showing time on and power.
#define SCR_ALT_DELAY 5000
bool showTimeOn;
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
  EEPROM.read(EEPROM.PageBase0+ ADDR, &savedEnergy1);
  EEPROM.read(EEPROM.PageBase0 + ADDR + 1, &savedEnergy2);

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
  lastSave = millis();
  pendingSaving = false;

  // Alternating screen
  lastScreenAlternate = millis();
  showTimeOn = false;

  // Setup WDG
  iwdg_init(IWDG_PRE_256, 10000); // Restart after 35.56ms if not responding
}

void loop()
{
  computeAverage();
  saveIfNeeded();
  handleOverflow();
  iwdg_feed();
  autosave();
}

// Auto save of energy counter every hour
void autosave()
{
  if ((millis() - lastSave) > AUTOSAVEDELAY)
  {
    lastSave = millis();
    pendingSaving = true; // Enable the save on power off (if it was disabled by any weird reason)
    saveEnergy();
  }
}

void saveEnergy()
{
  // Save energy counter multiply by 10000 using two SHORT (16bits) values;
  int energia = (int)(energy * 10000.0);
  uint16 low = (uint16)(energia & 0x0000FFFF);
  EEPROM.write(EEPROM.PageBase0 + ADDR, low);
  uint16 high = (uint16)(energia >> 16) & 0x0000FFFF;
  EEPROM.write(EEPROM.PageBase0 + ADDR + 1, high);
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
      saveEnergy();
      pendingSaving = false;
    }
  }
}

// computes average of electric values
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
  display.clearDisplay();

  if (CALIBRATE) // Calibration screen
  {
    // First line
    display.setCursor(0, 0);
    display.print(v, 1);
    display.print("V");
    // Second line
    display.setCursor(0, 24);
    display.print(i, 2);
    display.print("A");
  }
  else // Running screen
  {
    char tbs[5];
    char energyStr[12];
    char timeStr[9];

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
    if (millis() - lastScreenAlternate > SCR_ALT_DELAY)
    {
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
  }

  display.display();
}
