#include "TankParameters.h"
/*
  Class for handling tank parameters.
*/
// Constructor 
TankParameters::TankParameters(uint16 baseAdd){
  uint16 h, g, m, r;
  baseAddress = baseAdd;
  EEPROM.read(EEPROM.PageBase0+baseAddress, &height);
  EEPROM.read(EEPROM.PageBase0+baseAddress+4, &gap);
  EEPROM.read(EEPROM.PageBase0+baseAddress+8, &min);
  EEPROM.read(EEPROM.PageBase0+baseAddress+12, &restart);
}


int TankParameters::setHeight(int newValue){
  if (newValue > MIN_GAP && newValue < MAX_HEIGHT){
    height = newValue;
    EEPROM.write(EEPROM.PageBase0+baseAddress, height);
    return 0;
  } else {
    return -1;
  }  
}
int TankParameters::setGap(int newValue){
  if (newValue >= MIN_GAP && 
      newValue < height){
      gap = newValue;
      EEPROM.write(EEPROM.PageBase0+baseAddress+4, gap);
      return 0;
  } else {
    return -1;
  }  
}
int TankParameters::setRestart(int newValue){
  if (newValue > 0 && newValue < 100){
      restart = newValue;
      EEPROM.write(EEPROM.PageBase0+baseAddress+12, restart);
      return 0;
  } else {
    return -1;
  }  
}
int TankParameters::setMin(int newValue){
  if (newValue > 0 && newValue < 100){
      min = newValue;
      EEPROM.write(EEPROM.PageBase0+baseAddress+8, min);
      return 0;
  } else {
    return -1;
  }  
}

int TankParameters::getHeight(){
  //EEPROM.read(EEPROM.PageBase0+baseAddress, &height);
  return height;
}

int TankParameters::getGap(){
  //EEPROM.read(EEPROM.PageBase0+baseAddress+4, &gap);
  return gap;
}

int TankParameters::getMin(){
  //EEPROM.read(EEPROM.PageBase0+baseAddress+8, &min);
  return min;
}

int TankParameters::getRestart(){
  //EEPROM.read(EEPROM.PageBase0+baseAddress+12, &restart);
  return restart;
}
