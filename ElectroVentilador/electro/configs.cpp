#include "configs.h"

// Constructor
Configs::Configs(Adafruit_ST7735 *tft, int *st, Button *btn)
{
  _tft = tft;
  _btn = btn;
  lastTimePressed = 0;
  screen = DUMMY;
  screenTask = st;
  value = 0;
}

//--------------------------------------------------------------------
void Configs::init()
{
  uint16 temperature1, hyst, calib;

  lastTimePressed = 0;
  lastTimeChanged = 0;

  // Configure Memory
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize = 0x400;

  // Read initial values from memory
  EEPROM.read(EEPROM.PageBase0 + BASE_ADDRESS, &temperature1);
  temp1 = temperature1;
  EEPROM.read(EEPROM.PageBase0 + BASE_ADDRESS + 4, &hyst);
  hysteresis = hyst;
  EEPROM.read(EEPROM.PageBase0 + BASE_ADDRESS + 8, &calib);
  calibration = calib - abs(MIN_CAL_VAL);

  // validate configs
  validateConfigs();
  if (configurationNeeded)
  {
    startConfig();
  }
}

//--------------------------------------------------------------------
void Configs::validateConfigs()
{
  if (temp1 < MIN_TEMP_VAL || temp1 > MAX_TEMP_VAL ||
      hysteresis < MIN_HYST_VAL || hysteresis > MAX_HYST_VAL ||
      calibration < MIN_CAL_VAL || calibration > MAX_CAL_VAL)
  {
    configurationNeeded = true;
  }
}

//--------------------------------------------------------------------
void Configs::setPos(int position)
{
  // Delete old Square background
  _tft->drawRect(0, pos * 32, 160, 32, ST7735_BLACK);
  _tft->setCursor(0, 1);

  // Draw new Square background
  pos = position;
  _tft->drawRect(0, pos * 32, 160, 32, ST7735_BLUE);
  _tft->setCursor(0, 1);
  //drawMenu(text, nItems);
}

//--------------------------------------------------------------------
void Configs::prepareEditScreen(const char text[][STRING_LENGTH])
{
  char tbs[6];

  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
  _tft->setTextSize(2);
  _tft->fillScreen(ST7735_BLACK);
  _tft->setCursor(0, 0);
  _tft->println(text[pos]);
  switch (pos)
  {
  case 0:
    screen = TEMP1;
    value = MIN_TEMP_VAL;
    printValue(temp1);
    break;
  case 1:
    screen = HYST;
    value = MIN_HYST_VAL;
    printValue(hysteresis);
    break;
  case 2:
    screen = CALIBRAR;
    value = MIN_CAL_VAL;
    printValue(calibration);
    break;
  case 3:
    *screenTask = TEST;
    leaveMenus();
    break;
  default:
    break;
  }
}

//--------------------------------------------------------------------
void Configs::printLargeValue()
{
  // Clean
  _tft->setCursor(48, 48);
  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
  _tft->setTextSize(6);
  _tft->print("   ");

  // Print
  _tft->setCursor(48, 48);
  _tft->print(value);
}

//--------------------------------------------------------------------
void Configs::drawMenu(const char text[][STRING_LENGTH], int nItems)
{
  int i;

  _tft->setTextSize(2);
  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);

  for (i = 0; i < nItems; i++)
  {
    _tft->println(text[i]);
    switch (i)
    {
    case 0:
      printValue(temp1);
      break;
    case 1:
      printValue(hysteresis);
      break;
    case 2:
      printValue(calibration);
      break;
    default:
      break;
    }
  }
}

//--------------------------------------------------------------------
void Configs::printValue(int val)
{
  _tft->print(' ');
  _tft->print(val);
  _tft->print((char)247);
  _tft->println('C');
}

//--------------------------------------------------------------------
void Configs::startConfig()
{
  screen = MAIN;
  *screenTask = CONFIG;
  _tft->fillScreen(ST7735_BLACK);
  _tft->setCursor(0, 0);
  drawMenu(mainText, MAIN_ITEMS);
  setPos(0);
  validateConfigs();
  lastTimePressed = millis();
  _btn->clear();
}

//--------------------------------------------------------------------
void Configs::leaveMenus()
{
  screen = DUMMY;
  if (*screenTask != TEST)
  {
    *screenTask = PENDING;
  }
  _btn->clear();
}

//--------------------------------------------------------------------
void Configs::startTest()
{
  screen = DUMMY;
  *screenTask = TEST;
  _tft->fillScreen(ST7735_BLACK);
  _btn->clear();
}

//--------------------------------------------------------------------
void Configs::run()
{
  int position, out;

  // Screen sequence
  switch (screen)
  {
  case DUMMY:
    if (_btn->longPressed())
    {
      // Next State Logic
      startConfig();
    }
    break;
  case MAIN:
    // Current State Logic
    if (millis() - lastTimePressed > WAITING_TIME && !configurationNeeded)
    {
      leaveMenus();
    }
    if (millis() - lastTimeChanged > CHANGE_TIME)
    {
      int active_elements = MAIN_ITEMS;
      if (configurationNeeded)
      {
        active_elements--;
      }
      setPos((pos + 1) % active_elements);
      lastTimeChanged = millis();
    }
    if (_btn->shortPressed())
    {
      lastTimePressed = millis();
      prepareEditScreen(mainText);
    }
    break;
  case TEMP1:
    if (millis() - lastTimePressed > WAITING_TIME)
    {
      screen = MAIN;
      startConfig();
      lastTimePressed = millis();
    }
    if (millis() - lastTimeChanged > CHANGE_TIME)
    {
      if (++value > MAX_TEMP_VAL)
      {
        value = MIN_TEMP_VAL;
      }
      printLargeValue();
      lastTimeChanged = millis();
    }
    if (_btn->shortPressed())
    {
      lastTimePressed = millis();
      saveTemp();
      startConfig();
    }
    break;
  case HYST:
    if (millis() - lastTimePressed > WAITING_TIME)
    {
      screen = MAIN;
      startConfig();
      lastTimePressed = millis();
    }
    if (millis() - lastTimeChanged > CHANGE_TIME)
    {
      if (++value > MAX_HYST_VAL)
      {
        value = MIN_HYST_VAL;
      }
      printLargeValue();
      lastTimeChanged = millis();
    }
    if (_btn->shortPressed())
    {
      lastTimePressed = millis();
      saveHyst();
      startConfig();
    }
    break;
  case CALIBRAR:
    if (millis() - lastTimePressed > WAITING_TIME)
    {
      screen = MAIN;
      startConfig();
      lastTimePressed = millis();
    }
    if (millis() - lastTimeChanged > CHANGE_TIME)
    {
      if (++value > MAX_CAL_VAL)
      {
        value = MIN_CAL_VAL;
      }
      printLargeValue();
      lastTimeChanged = millis();
    }
    if (_btn->shortPressed())
    {
      lastTimePressed = millis();
      saveCalibration();
      startConfig();
    }
    break;
  default:
    break;
  }
}

//--------------------------------------------------------------------
int Configs::getTemp()
{
  return temp1;
}

//--------------------------------------------------------------------
int Configs::getHyst()
{
  return hysteresis;
}

//--------------------------------------------------------------------
int Configs::getCalibration()
{
  return calibration;
}

//--------------------------------------------------------------------
void Configs::saveTemp()
{
  EEPROM.write(EEPROM.PageBase0 + BASE_ADDRESS, (uint16)value);
  temp1 = value;
  validateConfigs();
}

//--------------------------------------------------------------------
void Configs::saveHyst()
{
  EEPROM.write(EEPROM.PageBase0 + BASE_ADDRESS + 4, (uint16)value);
  hysteresis = value;
  validateConfigs();
}

//--------------------------------------------------------------------
void Configs::saveCalibration()
{
  uint16 val = (uint16)(value + abs(MIN_CAL_VAL));
  EEPROM.write(EEPROM.PageBase0 + BASE_ADDRESS + 8, val);
  val = 0;
  EEPROM.read(EEPROM.PageBase0 + BASE_ADDRESS + 8, &val);
  calibration = value;
  validateConfigs();
}
