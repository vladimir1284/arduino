#include "hmi.h"

// Constructor
ThermHMI::ThermHMI(Adafruit_ST7735 *tft, int *st, Configs *cfgs, ElectroController *controller)
{
  _tft = tft;
  _cfgs = cfgs;
  _ctrl = controller;
  lastFanSpeed = FAN_STOP;
  lastTimeTempUdated = 0;
  lastTimeFanChanged = 0;
  pendingClear = false;
  screenTask = st;
  indicator_color = ST7735_WHITE;
  lastVoltage = 0;
}

//--------------------------------------------------------------------
void ThermHMI::showConfigs()
{
  // Prepare text
  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
  _tft->setTextSize(1);

  // Update
  int T = _cfgs->getTemp();
  int H = _cfgs->getHyst();

  _tft->setCursor(Xcfg1, Ycfg1);
  _tft->print(T);
  _tft->setCursor(Xcfg1 + DxCfg1, Ycfg1 - DyCfg1);
  _tft->print(T + H);
  _tft->setCursor(Xcfg1 + 2 * DxCfg1, Ycfg1 - 2 * DyCfg1);
  _tft->print(T + 2 * H);
}

//--------------------------------------------------------------------
void ThermHMI::drawBateryIcon(int color)
{
  // body
  _tft->fillRoundRect(Xbat, Ybat, Wbat, Hbat, Rbat, color);
  // Connectors
  _tft->fillRoundRect(Xbat + 4, Ybat - 5, 6, 6, 1, color);
  _tft->fillRoundRect(Xbat + 15, Ybat - 5, 6, 6, 1, color);
  _tft->fillRect(Xbat + 4, Ybat - 2, 21, 2, ST7735_BLACK);
  // Polarity
  _tft->fillRect(Xbat + 4, 1.5 * Ybat + 2, 6, 2, ST7735_BLACK);
  _tft->fillRect(Xbat + 15, 1.5 * Ybat + 2, 6, 2, ST7735_BLACK);
  _tft->fillRect(Xbat + 17, 1.5 * Ybat, 2, 6, ST7735_BLACK);
}

//--------------------------------------------------------------------
void ThermHMI::updateVoltage(float voltage)
{
  int color;

  // Draw batery icon and get the color
  if (voltage < VOLT_MIN)
  {
    if (abs(voltage - lastVoltage) > 0.1)
    {
      drawBateryIcon(ST7735_WHITE);
      color = ST7735_WHITE;
    }
  }
  else if (voltage > VOLT_MAX)
  {
    blinkBatery();
    color = ST7735_RED;
  }
  else
  {
    if (abs(voltage - lastVoltage) > 0.1)
    {
      drawBateryIcon(ST7735_GREEN);
      color = ST7735_GREEN;
    }
  }

  // Update text
  if (abs(voltage - lastVoltage) > 0.1)
  {
    _tft->setTextColor(color, ST7735_BLACK);
    _tft->setTextSize(2);

    // Clear
    _tft->setCursor(Xbat + Wbat + TEXTgap, Ybat + TEXTgap);
    _tft->print("     ");

    // Update
    _tft->setCursor(Xbat + Wbat + TEXTgap, Ybat + TEXTgap);
    _tft->print(voltage, 1);
    _tft->print('V');

    lastVoltage = voltage;
  }
}

//--------------------------------------------------------------------
void ThermHMI::drawHMItemplate()
{
  _tft->fillScreen(ST7735_BLACK);
  drawIcon();
  updateVoltage(0); // Forcing drawing with a different value
  drawFan();
  showConfigs();
  acON = false;
  overPressureON = false;
  alarmON = false;
  speed0ON = false;
  speed1ON = false;
}

//--------------------------------------------------------------------
void ThermHMI::drawFan()
{
  _tft->fillRect(Xfan, Yfan, 48, 48, ST7735_BLACK);
  _tft->drawBitmap(Xfan, Yfan, fan0, Wfan, Hfan, indicator_color);
}

//--------------------------------------------------------------------
void ThermHMI::blinkBatery()
{
  if (millis() - lastBateryChanged > BatBlinkDelay)
  {
    if (batON)
    {
      drawBateryIcon(ST7735_BLACK);
      batON = false;
    }
    else
    {
      drawBateryIcon(ST7735_RED);
      batON = true;
    }
  }
}

//--------------------------------------------------------------------
void ThermHMI::animateFan(int speed)
{
  int fan_delay;
  if (speed == FAN_SPEED_1)
  {
    fan_delay = SLOWdelay;
  }
  else
  {
    fan_delay = FASTdelay;
  }
  if (millis() - lastTimeFanChanged > fan_delay)
  {
    _tft->fillRect(Xfan, Yfan, 48, 48, ST7735_BLACK);
    if (fan == 0)
    {
      fan = 1;
      _tft->drawBitmap(Xfan, Yfan, fan0, Wfan, Hfan, indicator_color);
    }
    else
    {
      fan = 0;
      _tft->drawBitmap(Xfan, Yfan, fan1, Wfan, Hfan, indicator_color);
    }
    lastTimeFanChanged = millis();
  }
}

//--------------------------------------------------------------------
int ThermHMI::getColor(float tempValue)
{
  int color;
  if (tempValue < (_cfgs->getTemp() - 4 * _cfgs->getHyst()))
  {
    color = ST7735_WHITE;
  }
  else if (tempValue < _cfgs->getTemp())
  {
    color = ST7735_GREEN;
  }
  else if (tempValue < (_cfgs->getTemp() + _cfgs->getHyst()))
  {
    color = ORANGE_COLOR;
  }
  else
  {
    color = ST7735_RED;
  }
  return color;
}

//--------------------------------------------------------------------
void ThermHMI::update()
{
  int tempValue = _ctrl->getTemperature();
  int fanSpeed = _ctrl->getFanSpeed();
  int error_code = _ctrl->getErrorCode();
  float voltage = _ctrl->getVoltage();
  int color = 0;

  if (error_code == NO_ERROR)
  {
    last_error_shown = NO_ERROR;
    if (*screenTask == ERROR)
    {
      *screenTask = PENDING;
    }
    if (*screenTask == WORK || *screenTask == TEST)
    {
      if (millis() - lastTimeTempUdated > TEMP_DELAY)
      {
        color = getColor(tempValue);
        if (color != indicator_color)
        {
          indicator_color = color;
          drawIcon();
          drawFan();
        }
        if (tempValue != current_value)
        {
          updateTemp(tempValue);
        }
        updateVoltage(voltage);
        updateStatusLabels();
        lastTimeTempUdated = millis();
      }

      if (fanSpeed != FAN_STOP)
      {
        animateFan(fanSpeed);
      }
      else if (lastFanSpeed != FAN_STOP)
      {
        // Update the fan to a static image after stop
        drawFan();
      }
    }
    else
    {
      // Ensure that temperature is printed after screen task change
      current_value = 0;
    }
  }
  else
  {
    // Error detected
    showError(error_code);
  }
  lastFanSpeed = fanSpeed;
}

//--------------------------------------------------------------------
void ThermHMI::showAll()
{
  //drawHMItemplate();
  updateTemp(99);
  showStatusLabels();
  updateVoltage(12.4);
}

//--------------------------------------------------------------------
void ThermHMI::showStatusLabels()
{
  updateLabel(&acON, true, ST7735_WHITE,
              AClabel, Xlabel, Ylabel);
  updateLabel(&overPressureON, true, ST7735_WHITE,
              APlabel, Xlabel + DxLabel, Ylabel);
  updateLabel(&alarmON,true, ST7735_RED,
              ATlabel, Xlabel + 2 * DxLabel, Ylabel);
  // Fan speeds
  updateLabel(&speed0ON, true,
              ST7735_YELLOW, Ipaso, Xlabel + DxLabel / 2, Ylabel - DyLabel);
  updateLabel(&speed1ON, true, ST7735_RED,
              IIpaso, Xlabel + 3 * DxLabel / 2, Ylabel - DyLabel);
}

//--------------------------------------------------------------------
void ThermHMI::updateStatusLabels()
{
  updateLabel(&acON, _ctrl->getACstatus(), ST7735_WHITE,
              AClabel, Xlabel, Ylabel);
  updateLabel(&overPressureON, _ctrl->getOverPressure(), ST7735_WHITE,
              APlabel, Xlabel + DxLabel, Ylabel);
  updateLabel(&alarmON, _ctrl->getOverTemperature(), ST7735_RED,
              ATlabel, Xlabel + 2 * DxLabel, Ylabel);
  // Fan speeds
  updateLabel(&speed0ON, (_ctrl->getFanSpeed() == SPEED0 || _ctrl->getFanSpeed() == SPEED1),
              ST7735_YELLOW, Ipaso, Xlabel + DxLabel / 2, Ylabel - DyLabel);
  updateLabel(&speed1ON, _ctrl->getFanSpeed() == SPEED1, ST7735_RED,
              IIpaso, Xlabel + 3 * DxLabel / 2, Ylabel - DyLabel);
}

//--------------------------------------------------------------------
void ThermHMI::updateLabel(bool *previousSatus, bool status,
                           int color, char *label, int x, int y)
{
  if (*previousSatus)
  {
    if (!status)
    {
      _tft->setCursor(x, y);
      _tft->setTextSize(1);
      _tft->setTextColor(color, ST7735_BLACK);
      _tft->print("  ");
      *previousSatus = false;
    }
  }
  else
  {
    if (status)
    {
      _tft->setCursor(x, y);
      _tft->setTextColor(color, ST7735_BLACK);
      _tft->setTextSize(1);
      _tft->print(label);
      *previousSatus = true;
    }
  }
}

//--------------------------------------------------------------------
void ThermHMI::showError(int error_code)
{
  *screenTask = ERROR;
  if (last_error_shown != error_code)
  {
    _tft->setTextColor(ST7735_RED, ST7735_BLACK);
    _tft->setTextSize(2);
    _tft->fillScreen(ST7735_BLACK);
    _tft->setCursor(0, 0);
    _tft->println();
    _tft->println(" Sensor en");
    switch (error_code)
    {
    case SHORT_CIRCUIT:
      _tft->println("  Corto");
      _tft->println(" Circuito");
      break;
    case OPEN_CIRCUIT:
      _tft->println(" Circuito");
      _tft->println(" Abierto");
      break;
    default:
      break;
    }

    last_error_shown = error_code;
  }
}

//--------------------------------------------------------------------
void ThermHMI::updateTemp(int value)
{
  int xShift = 64;

  drawBar(value);
  current_value = value;

  if (value > 99)
  {
    xShift += 25;
    pendingClear = true;
  }
  else
  {
    if (pendingClear)
    {
      _tft->setCursor(Xicon - 89, Yicon - 16);
      _tft->print("   ");
      pendingClear = false;
    }
  }
  //_tft->drawChar()
  _tft->setTextSize(4);
  _tft->setTextColor(indicator_color, ST7735_BLACK);
  _tft->setCursor(Xicon - xShift, Yicon - 16);
  _tft->print(value);
}

//--------------------------------------------------------------------
void ThermHMI::drawBar(int temp)
{
  int i, color, k;
  float iValue;

  for (i = 0; i < nRegs; i++)
  {
    iValue = i * (MAXindicator - MINindicator) / nRegs + MINindicator;
    if (iValue > temp)
    {
      color = ST7735_BLACK;
    }
    else
    {
      color = getColor(iValue);
    }
    for (k = regPtrs[i]; k < regPtrs[i + 1]; k++)
    {
      _tft->drawPixel(Xcoords[k], Ycoords[k], color);
    }
  }
}

//--------------------------------------------------------------------
void ThermHMI::drawIcon()
{
  // --------------- Draw icon -------------------------
  // Circle
  _tft->fillCircle(Xicon, Yicon, Rtherm, indicator_color);
  // Bulb
  _tft->fillRect(Xicon - Wtherm / 2, Yicon - Rtherm - Htherm, Wtherm, Htherm, indicator_color);
  _tft->fillCircle(Xicon, Yicon - Rtherm - Htherm, Wtherm / 2, indicator_color);
  // Marks
  _tft->fillRect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + SPACEmarks / 2, Lmarks, Wmarks, indicator_color);
  _tft->fillRect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + 3 * SPACEmarks / 2, Lmarks, Wmarks, indicator_color);
  _tft->fillRect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + 5 * SPACEmarks / 2, Lmarks, Wmarks, indicator_color);
  // right hand wave
  _tft->fillRect(Xicon + SPACEwaves, Yicon + Hwaves, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon + SPACEwaves + Wwaves, Yicon, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon + SPACEwaves + 2 * Wwaves, Yicon + Hwaves, Hwaves, Hwaves, indicator_color);
  // left hand wave
  _tft->fillRect(Xicon - SPACEwaves - Wwaves, Yicon + Hwaves, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon - SPACEwaves - 2 * Wwaves, Yicon, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon - SPACEwaves - 2 * Wwaves - Hwaves, Yicon + Hwaves, Hwaves, Hwaves, indicator_color);
  // left hand wave
  _tft->fillRect(Xicon - 3.5 * Wwaves + Hwaves, Yicon + SPACEwaves + 1, Hwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon - 2.5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon - 1.5 * Wwaves, Yicon + SPACEwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon - .5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon + .5 * Wwaves, Yicon + SPACEwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon + 1.5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->fillRect(Xicon + 2.5 * Wwaves, Yicon + SPACEwaves + 1, Hwaves, Hwaves, indicator_color);
  // ----- Celcius Degrees -------------
  _tft->setTextSize(1);
  _tft->setTextColor(indicator_color, ST7735_BLACK);
  _tft->setCursor(Xicon - 16, Yicon - 13);
  _tft->print((char)247);
  _tft->println('C');
}
