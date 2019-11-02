#include "hmi.h"

// Constructor
ThermHMI::ThermHMI(Adafruit_ST7735 *tft, int *st, Configs *cfgs)
{
  _tft = tft;
  _cfgs = cfgs;
  lastTimeFanChanged = 0;
  lastTimeTempUdated = 0;
  pendingClear = false;
  screenTask = st;
}

void ThermHMI::drawHMItemplate(int color)
{
  drawIcon(ST7735_WHITE);
  drawFan(ST7735_WHITE);
  drawMarks(ST7735_WHITE);
}

void ThermHMI::drawFan(int color)
{
  _tft->fillRect(Xfan, Yfan, 48, 48, ST7735_BLACK);
  _tft->drawBitmap(Xfan, Yfan, fan0, Wfan, Hfan, color);
}

void ThermHMI::animateFan(int speed)
{
  int fan_delay, color;
  if (speed == FAN_SPEED_1)
  {
    fan_delay = SLOWdelay;
    color = ST7735_WHITE;
  }
  else
  {
    fan_delay = FASTdelay;
    color = ST7735_BLUE;
  }
  if (millis() - lastTimeFanChanged > fan_delay)
  {
    _tft->fillRect(Xfan, Yfan, 48, 48, ST7735_BLACK);
    if (fan == 0)
    {
      fan = 1;
      _tft->drawBitmap(Xfan, Yfan, fan0, Wfan, Hfan, color);
    }
    else
    {
      fan = 0;
      _tft->drawBitmap(Xfan, Yfan, fan1, Wfan, Hfan, color);
    }
    lastTimeFanChanged = millis();
  }
}

void ThermHMI::update(int tempValue, int fanSpeed)
{
  if (*screenTask == WORK)
  {
    if (millis() - lastTimeTempUdated > TEMP_DELAY)
    {
      int color;
      if (tempValue < (_cfgs->getTemp() - 2 * _cfgs->getHyst()))
      {
        color = ST7735_WHITE;
      }
      else if (tempValue < (_cfgs->getTemp() + 2 * _cfgs->getHyst()))
      {
        color = ST7735_GREEN;
      }
      else
      {
        color = ST7735_RED;
      }
      updateTemp(tempValue, color);
      lastTimeTempUdated = millis();
    }

    if (fanSpeed != FAN_STOP)
    {
      animateFan(fanSpeed);
    }
    else if (millis() - lastTimeFanChanged < 3000)
    {
      drawFan(ST7735_WHITE);
      lastTimeFanChanged = 0;
    }
  }
}

void ThermHMI::drawNeedle(int value, int color)
{
  float angle;
  if (value > MAXindicator)
  {
    angle = 90 / 57.32;
  }
  else if (value < MINindicator)
  {
    angle = 0;
  }
  else
  {
    angle = 90 / 57.32 * (value - MINindicator) / (MAXindicator - MINindicator);
  }
  int x0 = GAUGEx - INNERneedle * cos(angle);
  int y0 = GAUGEy - INNERneedle * sin(angle);
  int x3 = GAUGEx - OUTERradius * cos(angle);
  int y3 = GAUGEy - OUTERradius * sin(angle);
  int x1 = x0 - NEEDLEbase * sin(angle);
  int y1 = y0 + NEEDLEbase * cos(angle);
  int x2 = x0 + NEEDLEbase * sin(angle);
  int y2 = y0 - NEEDLEbase * cos(angle);

  _tft->fillTriangle(x3, y3, x1, y1, x2, y2, color);
}

void ThermHMI::updateTemp(int value, int color)
{
  int xShift = 64;

  // Clear needle
  drawNeedle(current_value, ST7735_BLACK);
  current_value = value;
  // Draw new needle
  drawMarks(ST7735_WHITE);
  drawNeedle(value, ST7735_WHITE);

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
  _tft->setTextColor(color, ST7735_BLACK);
  _tft->setCursor(Xicon - xShift, Yicon - 16);
  _tft->print(value);
}

void ThermHMI::drawMarks(int color)
{
  int i, x0, y0, x1, y1, x2, y2, x3, y3, out, in, nbeams;
  float angle, minAngle, maxAngle, deltaAngle;
  angle = MAXangle / (Nmarks - 1) / 57.32;
  // Marks
  for (i = 0; i < Nmarks; i++)
  {
    _tft->drawLine(GAUGEx - INNERradius * cos(i * angle), GAUGEy - INNERradius * sin(i * angle),
                   GAUGEx - OUTERradius * cos(i * angle), GAUGEy - OUTERradius * sin(i * angle),
                   color);
  }
  // Cool
  out = OUTERradius - 2;
  in = INNERradius + 1;
  x0 = GAUGEx - out * cos(angle / 6);
  y0 = GAUGEy - out * sin(angle / 6);
  x1 = GAUGEx - in * cos(angle / 6);
  y1 = GAUGEy - in * sin(angle / 6);
  x2 = GAUGEx - out * cos(2 * angle / 6);
  y2 = GAUGEy - out * sin(2 * angle / 6);
  x3 = GAUGEx - in * cos(2 * angle / 6);
  y3 = GAUGEy - in * sin(2 * angle / 6);
  _tft->fillTriangle(x0, y0, x1, y1 - 1, x2, y2 + 1, ST7735_WHITE);
  _tft->fillTriangle(x1, y1 - 1, x3, y3, x2, y2 + 1, ST7735_WHITE);

  // Hot
  x0 = GAUGEx - out * cos(22 * angle / 6);
  y0 = GAUGEy - out * sin(22 * angle / 6);
  x1 = GAUGEx - in * cos(22 * angle / 6);
  y1 = GAUGEy - in * sin(22 * angle / 6);
  x2 = GAUGEx - out * cos(23 * angle / 6);
  y2 = GAUGEy - out * sin(23 * angle / 6);
  x3 = GAUGEx - in * cos(23 * angle / 6);
  y3 = GAUGEy - in * sin(23 * angle / 6);
  _tft->fillTriangle(x0, y0, x1, y1 - 1, x2, y2 + 1, ST7735_RED);
  _tft->fillTriangle(x1 - 1, y1 - 1, x3, y3, x2, y2 + 1, ST7735_RED);

  // OK
  x0 = GAUGEx - out * cos(13 * angle / 6);
  y0 = GAUGEy - out * sin(13 * angle / 6);
  x1 = GAUGEx - in * cos(13 * angle / 6);
  y1 = GAUGEy - in * sin(13 * angle / 6);
  x2 = GAUGEx - out * cos(14 * angle / 6);
  y2 = GAUGEy - out * sin(14 * angle / 6);
  x3 = GAUGEx - in * cos(14 * angle / 6);
  y3 = GAUGEy - in * sin(14 * angle / 6);
  _tft->fillTriangle(x0 + 1, y0 + 2, x1, y1 + 1, x2 + 1, y2 + 1, ST7735_GREEN);
  _tft->fillTriangle(x1, y1 + 1, x3, y3, x2 + 1, y2 + 1, ST7735_GREEN);
}

void ThermHMI::drawIcon(int color)
{
  // --------------- Draw icon -------------------------
  // Circle
  _tft->fillCircle(Xicon, Yicon, Rtherm, color);
  // Bulb
  _tft->fillRect(Xicon - Wtherm / 2, Yicon - Rtherm - Htherm, Wtherm, Htherm, color);
  _tft->fillCircle(Xicon, Yicon - Rtherm - Htherm, Wtherm / 2, color);
  // Marks
  _tft->fillRect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + SPACEmarks / 2, Lmarks, Wmarks, color);
  _tft->fillRect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + 3 * SPACEmarks / 2, Lmarks, Wmarks, color);
  _tft->fillRect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + 5 * SPACEmarks / 2, Lmarks, Wmarks, color);
  // right hand wave
  _tft->fillRect(Xicon + SPACEwaves, Yicon + Hwaves, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon + SPACEwaves + Wwaves, Yicon, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon + SPACEwaves + 2 * Wwaves, Yicon + Hwaves, Hwaves, Hwaves, color);
  // left hand wave
  _tft->fillRect(Xicon - SPACEwaves - Wwaves, Yicon + Hwaves, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon - SPACEwaves - 2 * Wwaves, Yicon, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon - SPACEwaves - 2 * Wwaves - Hwaves, Yicon + Hwaves, Hwaves, Hwaves, color);
  // left hand wave
  _tft->fillRect(Xicon - 3.5 * Wwaves + Hwaves, Yicon + SPACEwaves + 1, Hwaves, Hwaves, color);
  _tft->fillRect(Xicon - 2.5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon - 1.5 * Wwaves, Yicon + SPACEwaves + 1, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon - .5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon + .5 * Wwaves, Yicon + SPACEwaves + 1, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon + 1.5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, color);
  _tft->fillRect(Xicon + 2.5 * Wwaves, Yicon + SPACEwaves + 1, Hwaves, Hwaves, color);
  // ----- Celcius Degrees -------------
  _tft->setTextSize(1);
  _tft->setTextColor(color, ST7735_BLACK);
  _tft->setCursor(Xicon - 16, Yicon - 13);
  _tft->print((char)247);
  _tft->println('C');
}
