#include "hmi.h"

// Constructor 
PumpHMI::PumpHMI(Adafruit_ST7735 *tft) 
{
  _tft = tft;
  _animationStage = 0;
}

//----------------------------------------------------------------------------------
void PumpHMI::drawHMItemplate(int16_t lowerMin, int16_t upperMin) {

  // Setup thresholds
  _lowerMin = lowerMin;
  _upperMin = upperMin;

  // Draw upper tank
  _tft->drawFastHLine(p1x, p1y, b1, ST7735_WHITE);
  _tft->drawFastVLine(p1x, p1y-h1, h1, ST7735_WHITE);
  _tft->drawFastVLine(p1x + b1, p1y-h1, h1, ST7735_WHITE);
  _tft->drawFastHLine(p1x, p1y - h1, (b1 - c1) / 2, ST7735_WHITE);
  _tft->drawFastHLine(p1x + c1 + (b1 - c1) / 2, p1y - h1, (b1 - c1) / 2, ST7735_WHITE);
  _tft->drawFastHLine(p1x + (b1 - c1) / 2, p1y - h1 - d, c1, ST7735_WHITE);
  _tft->drawFastVLine(p1x + (b1 - c1) / 2, p1y - h1 - d, d, ST7735_WHITE);
  _tft->drawFastVLine(p1x + (b1 + c1) / 2, p1y - h1 - d, d, ST7735_WHITE);

  // Draw lower tank
  _tft->drawFastHLine(p2x, p2y, b2, ST7735_WHITE);
  _tft->drawFastVLine(p2x, p2y-h2, h2, ST7735_WHITE);
  _tft->drawFastVLine(p2x + b2, p2y-h2, h2, ST7735_WHITE); 

  // Draw pump
  _tft->drawCircle(p3x, p3y, din/2, ST7735_WHITE);
  _tft->drawCircle(p3x, p3y, dout/2, ST7735_WHITE);
  _tft->fillRect(p3x-dout/2, p3y-d/2, 18, d, ST7735_BLACK);
  _tft->fillRect(p3x-d/2, p3y-dout/2, d, 4, ST7735_BLACK);

  // Draw upper pipe  
  _tft->drawFastHLine(p1x+b1, p1y-h1+d, p3x+d/2-p1x-b1, ST7735_WHITE);
  _tft->drawFastHLine(p1x+b1, p1y-h1+2*d, p3x-d/2-p1x-b1, ST7735_WHITE);
  _tft->drawFastVLine(p3x+d/2, p1y-h1+d, (p3y-dout/2)-(p1y-h1+d)+2, ST7735_WHITE);
  _tft->drawFastVLine(p3x-d/2, p1y-h1+2*d, (p3y-dout/2)-(p1y-h1+2*d)+2, ST7735_WHITE);
  
  // Draw lower pipe  
  _tft->drawFastHLine(p3x-dout-din/2, p3y-d/2, dout+2, ST7735_WHITE);
  _tft->drawFastHLine(p3x-dout-din/2+d, p3y+d/2, dout-d+2, ST7735_WHITE);
  _tft->drawFastVLine(p3x-dout-din/2, p3y-d/2, 71, ST7735_WHITE);
  _tft->drawFastVLine(p3x-dout-din/2+d, p3y+d/2, 63, ST7735_WHITE);
  
}

//----------------------------------------------------------------------------------
void PumpHMI::updateLowerTankLevel(int16_t percent){
  int16_t h, color;
  h = percent*(h2-2)/100;
  // Restore black
  _tft->fillRect(p2x+1, p2y-1-h2, b2-1, h2, ST7735_BLACK);
  
  // Draw the current level
  if (percent < _lowerMin){
    color = ST7735_RED;
  } else {
    color = ST7735_BLUE;
  }
  _tft->fillRect(p2x+1, p2y-1-h, b2-1, h, color);  

  // Restore pipes
  _tft->drawFastVLine(p3x-dout-din/2, h0-h2, h2, ST7735_WHITE);
  _tft->drawFastVLine(p3x-dout-din/2+d, h0-h2, h2, ST7735_WHITE);
}

//----------------------------------------------------------------------------------
void PumpHMI::updateUpperTankLevel(int16_t percent){
  int16_t h, color;
  h = percent*(h1-2)/100;
  // Restore black
  _tft->fillRect(p1x+1, p1y+1-h1, b1-1, h1-2, ST7735_BLACK);
  
  // Draw the current level
  if (percent < _upperMin){
    color = ST7735_RED;
  } else {
    color = ST7735_BLUE;
  }
  _tft->fillRect(p1x+1, p1y-1-h, b1-1, h, color);  
}

//----------------------------------------------------------------------------------
void PumpHMI::animate(){  
  // Delete old scuares
  _tft->fillRect(Xup[_animationStage], Yup[_animationStage], d-1, d-1, ST7735_BLACK);  
  _tft->fillRect(Xdown[_animationStage], Ydown[_animationStage], d-1, d-1, ST7735_BLACK); 
  _tft->fillCircleHelper(p3x,p3y,din/2-1,_animationStage,0,ST7735_BLACK);
  
  // Draw new scuares
  _animationStage=(_animationStage+1) % 3; // Only 3 stages
  _tft->fillRect(Xup[_animationStage], Yup[_animationStage], d-1, d-1, ST7735_BLUE);  
  _tft->fillRect(Xdown[_animationStage], Ydown[_animationStage], d-1, d-1, ST7735_BLUE); 
  _tft->fillCircleHelper(p3x,p3y,din/2-1,_animationStage,0,ST7735_BLUE);
}
