#include "configs.h"

// Constructor
Configs::Configs(TTVout *tft, int *st, Button *btn)
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
  uint16 temperature1, hyst, calib, voltCalib;

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
  calibration = calib - BASE_SHIFT;
  EEPROM.read(EEPROM.PageBase0 + BASE_ADDRESS + 12, &voltCalib);
  voltCalibration = voltCalib - BASE_SHIFT;

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
      calibration < -20 || calibration > 20 ||
      voltCalibration < -20 || voltCalibration > 20)
  {
    configurationNeeded = true;
  }
}

//--------------------------------------------------------------------
void Configs::setPos(int position)
{
  // Delete old Square background
  _tft->draw_rect(0, pos * RECT_HEIGHT, RECT_WIDTH, RECT_HEIGHT, BLACK);
  _tft->set_cursor(0, 2);

  // Draw new Square background
  pos = position;
  _tft->draw_rect(0, pos * RECT_HEIGHT, RECT_WIDTH, RECT_HEIGHT, WHITE);
  _tft->set_cursor(0, 2);
  //drawMenu(text, nItems);
}

//--------------------------------------------------------------------
void Configs::prepareEditScreen(const char text[][STRING_LENGTH])
{
  char tbs[6];

  _tft->select_font(font6x8);
  _tft->clear_screen();
  _tft->set_cursor(0, 1);
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
    screen = VOLTAGE;
    value = MIN_CAL_VOLT;
    printVoltValue(voltCalibration);
    break;
  case 4:
    *screenTask = TEST;
    leaveMenus();
    break;
  default:
    break;
  }
}

//--------------------------------------------------------------------
void Configs::printLargeVoltValue()
{
  // Clean
  _tft->set_cursor(48, 48);
  _tft->select_font(font8x8);
  _tft->print("    ");

  // Print
  _tft->set_cursor(48, 48);
  _tft->print((float)value / 10.0, 1);
}
//--------------------------------------------------------------------
void Configs::printLargeValue()
{
  // Clean
  _tft->set_cursor(48, 48);
  _tft->select_font(font8x8);
  _tft->print("   ");

  // Print
  _tft->set_cursor(48, 48);
  _tft->print(value);
}

//--------------------------------------------------------------------
void Configs::drawMenu(const char text[][STRING_LENGTH], int nItems)
{
  int i;

  _tft->select_font(font6x8);

  for (i = 0; i < nItems; i++)
  {
    _tft->println(text[i]);
  }
}

//--------------------------------------------------------------------
void Configs::printVoltValue(int val)
{
  _tft->print(' ');
  _tft->print((float)val / 10.0);
  _tft->println('V');
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
  _tft->clear_screen();
  _tft->set_cursor(0, 1);
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
  _tft->clear_screen();
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
  case VOLTAGE:
    if (millis() - lastTimePressed > WAITING_TIME)
    {
      screen = MAIN;
      startConfig();
      lastTimePressed = millis();
    }
    if (millis() - lastTimeChanged > CHANGE_TIME)
    {
      if (++value > MAX_CAL_VOLT)
      {
        value = MIN_CAL_VOLT;
      }
      printLargeVoltValue();
      lastTimeChanged = millis();
    }
    if (_btn->shortPressed())
    {
      lastTimePressed = millis();
      saveVoltCalibration();
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
int Configs::getVoltCalibration()
{
  return voltCalibration;
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
  if (value == 0) // Reset calibration
  {
    calibration = 0;
  }
  else
  {
    calibration += value; // Shift current calibration
  }
  uint16 val = (uint16)(calibration + BASE_SHIFT);
  EEPROM.write(EEPROM.PageBase0 + BASE_ADDRESS + 8, val);
  validateConfigs();
}

//--------------------------------------------------------------------
void Configs::saveVoltCalibration()
{
  if (value == 0) // Reset calibration
  {
    voltCalibration = 0;
  }
  else
  {
    voltCalibration += value; // Shift current calibration
  }
  uint16 val = (uint16)(voltCalibration + BASE_SHIFT);
  EEPROM.write(EEPROM.PageBase0 + BASE_ADDRESS + 12, val);
  validateConfigs();
}