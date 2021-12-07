#include "hmi.h"

// Constructor
ThermHMI::ThermHMI(TTVout *tft, GPS *gps, RPM *rpm,int *st, Configs *cfgs, ElectroController *controller)
{
  _tft = tft;
  _gps = gps;
  _rpm = rpm;
  _cfgs = cfgs;
  _ctrl = controller;
  lastFanSpeed = FAN_STOP;
  lastTimeTempUdated = 0;
  lastTimeFanChanged = 0;
  pendingClear = false;
  dots = false;
  screenTask = st;
  indicator_color = WHITE;
  lastVoltage = 0;
  lastRPM = 0;
  lastSpeed = 0;
}

//--------------------------------------------------------------------
void ThermHMI::showConfigs()
{
  // Prepare text
  _tft->select_font(font6x8);

  // Update
  int T = _cfgs->getTemp();
  int H = _cfgs->getHyst();

  _tft->set_cursor(Xcfg1, Ycfg1);
  _tft->print(T);
  _tft->set_cursor(Xcfg1 + DxCfg1, Ycfg1 - DyCfg1);
  _tft->print(T + H);
  _tft->set_cursor(Xcfg1 + 2 * DxCfg1, Ycfg1 - 2 * DyCfg1);
  _tft->print(T + 2 * H);
}

//--------------------------------------------------------------------
void ThermHMI::drawBateryIcon(int color)
{
  // body
  _tft->draw_rect(Xbat, Ybat, Wbat, Hbat, WHITE, color);
  // Connectors
  _tft->draw_rect(Xbat + 4, Ybat - 5, 6, 6, WHITE, color);
  _tft->draw_rect(Xbat + 15, Ybat - 5, 6, 6, WHITE, color);
  _tft->draw_rect(Xbat + 4, Ybat - 2, 21, 2, BLACK, BLACK);
  // Polarity
  _tft->draw_rect(Xbat + 4, 1.5 * Ybat + 2, 6, 2, !color);
  _tft->draw_rect(Xbat + 15, 1.5 * Ybat + 2, 6, 2, !color);
  _tft->draw_rect(Xbat + 17, 1.5 * Ybat, 2, 6, !color);
}

//--------------------------------------------------------------------
void ThermHMI::updateSpeed(int speed)
{
  int xPos = Xspeed;

  // speed = random(120);
  // Update text
  if (speed != lastSpeed)
  {
    _tft->select_font(Arial_round_16x24);

    // Clear
    _tft->set_cursor(Xspeed, Yspeed);
    _tft->print("000");

    // Update
    if (speed < 100)
    {
      if (speed > 9)
      {
        xPos += 16;
      }
      else
      {
        xPos += 32;
      }
    }
    _tft->set_cursor(xPos, Yspeed);
    _tft->print(speed);

    lastSpeed = speed;
  }
}

//--------------------------------------------------------------------
void ThermHMI::updateRPM(int rpm)
{
  int xPos = Xrpm;
  // rpm = random(5000);

  // Update text
  if (1) //abs(rpm - lastRPM) > 10)
  {
    _tft->select_font(font6x8);

    // Clear
    _tft->set_cursor(Xrpm, Yrpm);
    _tft->print("    ");

    // Update
    if (rpm < 1000)
    {
      if (rpm > 99)
      {
        xPos += 6;
      }
      else
      {
        if (rpm > 9)
        {
          xPos += 12;
        }
        else
        {
          xPos += 18;
        }
      }
    }

    drawBar(rpm);

    _tft->set_cursor(xPos, Yrpm);
    _tft->print(rpm);

    lastRPM = rpm;
  }
}

//--------------------------------------------------------------------
void ThermHMI::updateVoltage(float voltage)
{
  int color;

  // Draw batery icon and get the color
  if (voltage < VOLT_MIN || voltage > VOLT_MAX)
  {
    blinkBatery();
    color = WHITE;
  }
  else
  {
    if (abs(voltage - lastVoltage) > 0.1)
    {
      drawBateryIcon(WHITE);
      color = WHITE;
    }
  }

  // Update text
  if (abs(voltage - lastVoltage) > 0.1)
  {
    _tft->select_font(font6x8);

    // Clear
    _tft->set_cursor(Xbat + Wbat + TEXTgap, Ybat + TEXTgap);
    _tft->print("     ");

    // Update
    _tft->set_cursor(Xbat + Wbat + TEXTgap, Ybat + TEXTgap);
    _tft->print(voltage, 1);
    _tft->print('V');

    lastVoltage = voltage;
  }
}

//--------------------------------------------------------------------
void ThermHMI::printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  char sz[32];

  if (d.isValid())
  {
    _tft->select_font(font6x8);
    // Clear
    _tft->set_cursor(Xdate, Ydatetime);
    _tft->print("         ");

    // Update
    _tft->set_cursor(Xdate, Ydatetime);

    sprintf(sz, "%02d/%02d/%02d ", d.day(), d.month(), d.year());
    // sprintf(sz, "09/08/20 ");
    _tft->print(sz);
  }

  if (t.isValid())
  {
    _tft->select_font(font6x8);
    // Clear
    _tft->set_cursor(Xtime, Ydatetime);
    _tft->print("     ");

    // Update
    _tft->set_cursor(Xtime, Ydatetime);
    if (dots)
    {
      sprintf(sz, "%02d %02d", t.hour(), t.minute());
      // sprintf(sz, "10 23");
      dots = false;
    }
    else
    {
      sprintf(sz, "%02d:%02d", t.hour(), t.minute());
      // sprintf(sz, "10:23");
      dots = true;
    }
    _tft->print(sz);
  }
}

//--------------------------------------------------------------------
void ThermHMI::drawHMItemplate()
{
  _tft->clear_screen();
  drawIcon();
  updateVoltage(0); // Forcing drawing with a different value
  // drawFan();
  drawUnits();
  showConfigs();
  acON = false;
  overPressureON = false;
  alarmON = false;
  speed0ON = false;
  speed1ON = false;
}

//--------------------------------------------------------------------
void ThermHMI::drawUnits()
{
  _tft->select_font(font6x8);

  _tft->set_cursor(Xspeed + 48, Yspeed + 12);
  _tft->print("Km/h");

  _tft->set_cursor(Xrpm, Yrpm + 10);
  _tft->print(" rpm");
}

//--------------------------------------------------------------------
void ThermHMI::drawFan()
{
  _tft->draw_rect(Xfan, Yfan, 48, 48, BLACK);
  _tft->bitmap(Xfan, Yfan, fan0, 0, Wfan, Hfan);
}

//--------------------------------------------------------------------
void ThermHMI::blinkBatery()
{
  if (millis() - lastBateryChanged > BatBlinkDelay)
  {
    if (batON)
    {
      drawBateryIcon(BLACK);
      batON = false;
    }
    else
    {
      drawBateryIcon(WHITE);
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
    _tft->draw_rect(Xfan, Yfan, 48, 48, BLACK);
    if (fan == 0)
    {
      fan = 1;
      _tft->bitmap(Xfan, Yfan, fan0, 0, Wfan, Hfan);
    }
    else
    {
      fan = 0;
      _tft->bitmap(Xfan, Yfan, fan1, 0, Wfan, Hfan);
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
    color = WHITE;
  }
  else if (tempValue < _cfgs->getTemp())
  {
    color = WHITE;
  }
  else if (tempValue < (_cfgs->getTemp() + _cfgs->getHyst()))
  {
    color = WHITE;
  }
  else
  {
    color = WHITE;
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
          //drawFan();
        }
        if (tempValue != current_value)
        {
          updateTemp(tempValue);
        }
        updateVoltage(voltage);
        updateSpeed(_gps->gps.speed.kmph());
        updateRPM(_rpm->getRPM());
        printDateTime(_gps->gps.date, _gps->gps.time);
        updateStatusLabels();
        lastTimeTempUdated = millis();
      }

      if (fanSpeed != FAN_STOP)
      {
        //animateFan(fanSpeed);
      }
      else if (lastFanSpeed != FAN_STOP)
      {
        // Update the fan to a static image after stop
        //drawFan();
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
  // updateLabel(&acON, true, WHITE,
  //             AClabel, Xlabel, Ylabel);
  // updateLabel(&overPressureON, true, WHITE,
  //             APlabel, Xlabel + DxLabel, Ylabel);
  updateLabel(&alarmON, true, WHITE,
              ATlabel, Xlabel + 2 * DxLabel, Ylabel);
  // Fan speeds
  // animateFan(SLOWdelay);
  updateLabel(&speed0ON, true,
              WHITE, Ipaso, Xlabel + DxLabel / 2, Ylabel - DyLabel);
  updateLabel(&speed1ON, true, WHITE,
              IIpaso, Xlabel + 3 * DxLabel / 2, Ylabel - DyLabel);

  // Speed
  updateSpeed(100);

  // RPM
  updateRPM(3000);

  // DateTime
  char sz[32];
  _tft->set_cursor(Xdate, Ydatetime);
  sprintf(sz, "09/08/20 ");
  _tft->print(sz);
  _tft->set_cursor(Xtime, Ydatetime);
  sprintf(sz, "10:23");
  _tft->print(sz);
}

//--------------------------------------------------------------------
void ThermHMI::updateStatusLabels()
{
  // updateLabel(&acON, _ctrl->getACstatus(), WHITE,
  //             AClabel, Xlabel, Ylabel);
  // updateLabel(&overPressureON, _ctrl->getOverPressure(), WHITE,
  //             APlabel, Xlabel + DxLabel, Ylabel);
  updateLabel(&alarmON, _ctrl->getOverTemperature(), WHITE,
              ATlabel, Xlabel + 2 * DxLabel, Ylabel);
  // Fan speeds
  updateLabel(&speed0ON, (_ctrl->getFanSpeed() == SPEED0 || _ctrl->getFanSpeed() == SPEED1),
              WHITE, Ipaso, Xlabel + DxLabel / 2, Ylabel - DyLabel);
  updateLabel(&speed1ON, _ctrl->getFanSpeed() == SPEED1, WHITE,
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
      _tft->set_cursor(x, y);
      _tft->select_font(font6x8);

      _tft->print("  ");
      *previousSatus = false;
    }
  }
  else
  {
    if (status)
    {
      _tft->set_cursor(x, y);

      _tft->select_font(font6x8);
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
    _tft->select_font(font6x8);
    _tft->clear_screen();
    _tft->set_cursor(0, 0);
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
  int xShift = 56;

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
      _tft->set_cursor(Xicon - 89, Yicon - 16);
      _tft->print("   ");
      pendingClear = false;
    }
  }

  _tft->select_font(Arial_round_16x24);
  _tft->set_cursor(Xicon - xShift, Yicon - 12);
  _tft->print(value);
}

//--------------------------------------------------------------------
void ThermHMI::drawBar(int value)
{
  int i, color, k;
  float iValue;

  for (i = 0; i < nRegs; i++)
  {
    iValue = i * (MAXindicator - MINindicator) / nRegs + MINindicator;
    if (iValue > value)
    {
      color = BLACK;
    }
    else
    {
      color = getColor(iValue);
    }
    for (k = regPtrs[i]; k < regPtrs[i + 1]; k++)
    {
      _tft->set_pixel(Xcoords[k] + 30, Ycoords[k] - 20, color);
    }
  }
}

//--------------------------------------------------------------------
void ThermHMI::drawIcon()
{
  // --------------- Draw icon -------------------------
  // Circle
  _tft->draw_circle(Xicon, Yicon, Rtherm, indicator_color, INVERT);
  // Bulb
  _tft->draw_rect(Xicon - Wtherm / 2, Yicon - Rtherm - Htherm, Wtherm, Htherm, indicator_color);
  _tft->draw_circle(Xicon, Yicon - Rtherm - Htherm, Wtherm / 2, indicator_color, INVERT);
  // Marks
  _tft->draw_rect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + SPACEmarks / 2, Lmarks, Wmarks, indicator_color);
  _tft->draw_rect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + 3 * SPACEmarks / 2, Lmarks, Wmarks, indicator_color);
  _tft->draw_rect(Xicon + Wtherm / 2, Yicon - Rtherm - Htherm + 5 * SPACEmarks / 2, Lmarks, Wmarks, indicator_color);
  // right hand wave
  _tft->draw_rect(Xicon + SPACEwaves, Yicon + Hwaves, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon + SPACEwaves + Wwaves, Yicon, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon + SPACEwaves + 2 * Wwaves, Yicon + Hwaves, Hwaves, Hwaves, indicator_color);
  // left hand wave
  _tft->draw_rect(Xicon - SPACEwaves - Wwaves, Yicon + Hwaves, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon - SPACEwaves - 2 * Wwaves, Yicon, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon - SPACEwaves - 2 * Wwaves - Hwaves, Yicon + Hwaves, Hwaves, Hwaves, indicator_color);
  // left hand wave
  _tft->draw_rect(Xicon - 3.5 * Wwaves + Hwaves, Yicon + SPACEwaves + 1, Hwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon - 2.5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon - 1.5 * Wwaves, Yicon + SPACEwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon - .5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon + .5 * Wwaves, Yicon + SPACEwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon + 1.5 * Wwaves, Yicon + SPACEwaves + Hwaves + 1, Wwaves, Hwaves, indicator_color);
  _tft->draw_rect(Xicon + 2.5 * Wwaves, Yicon + SPACEwaves + 1, Hwaves, Hwaves, indicator_color);
  // ----- Celcius Degrees -------------
  _tft->select_font(font6x8);
  _tft->set_cursor(Xicon - 22, Yicon - 17);
  _tft->print('o');
  _tft->set_cursor(Xicon - 16, Yicon - 13);
  _tft->println('C');
}
