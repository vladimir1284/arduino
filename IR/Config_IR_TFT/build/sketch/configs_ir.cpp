#include "configs_ir.h"

HardwareTimer timer(2);


// Constructor 
IRConfigs::IRConfigs(Adafruit_ST7735 *tft, bool *LOCK_SCREEN) {
  _tft = tft;
  lastTimePressed = 0;
  screen = DUMMY;
  lockScreen = LOCK_SCREEN;
}

void IRConfigs::timer2_init ()
{
    timer.pause();
    timer.setPrescaleFactor( ((F_CPU / F_INTERRUPTS)/8) - 1);
    timer.setOverflow(7);
    timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
    timer.attachCompare1Interrupt(TIM2_IRQHandler);
        // Refresh the timer's count, prescale, and overflow
    timer.refresh();

    // Start the timer counting
    timer.resume();
}
  
void IRConfigs::init()                                                       // Timer2 Interrupt Handler
{
// Initialize IR receiber
  irmp_init();   // initialize irmp
  timer2_init(); // initialize timer2

  // Configure Memory
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x400;

  // Tanks
  lowerTank = TankParameters(BASE_ADR_LOWER_TANK); 
  upperTank = TankParameters(BASE_ADR_UPPER_TANK); 
  currentTank = &lowerTank;
}
  
void IRConfigs::TIM2_IRQHandler()                                                       // Timer2 Interrupt Handler
{
  (void) irmp_ISR(); // call irmp ISR   
}

void IRConfigs::setPos(const char text[][STRING_LENGTH], int nItems, int position){
  pos = position;
  drawMenu(text, nItems);
}

void IRConfigs::drawTankParameters(){
  char tbs[6];
  newStr = "";

  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
  _tft->setTextSize(1);

  _tft->setCursor(75, 8);
  sprintf(tbs, "%3dcm", currentTank->getHeight());
  _tft->print(tbs);
  _tft->setCursor(75, 24);
  sprintf(tbs, "%3dcm", currentTank->getGap());
  _tft->print(tbs);
  _tft->setCursor(75, 40);
  sprintf(tbs, " %2d%c", currentTank->getRestart(),0x25);
  _tft->print(tbs);
  _tft->setCursor(75, 56);
  sprintf(tbs, " %2d%c", currentTank->getMin(),0x25);
  _tft->print(tbs);
}

void IRConfigs::prepareEditScreen(){
  char tbs[6];
  char line[20];

  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
  _tft->setTextSize(1);
  _tft->fillScreen(ST7735_BLACK);
  _tft->setCursor(0, 0);

  switch (pos){
    case 0:
      sprintf(tbs, "%3dcm", currentTank->getHeight());  
      break;
    case 1:
      sprintf(tbs, "%3dcm", currentTank->getGap());
      break;
    case 2:
      sprintf(tbs, " %2d%c", currentTank->getRestart(),0x25); 
      break;
    case 3:
      sprintf(tbs, " %2d%c", currentTank->getMin(),0x25);
      break;
    default:
      break;
  }
  sprintf(line, "%s %s", tankCfgText[pos], tbs);
  _tft->print(line);

}

void IRConfigs::printParameterValue(int bg_color){
  // Clean
  _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
  _tft->setTextSize(2);
  _tft->print("          ");
  // Print
  _tft->setTextColor(ST7735_WHITE, bg_color);
  _tft->setTextSize(2);
  _tft->setCursor(0, 16);
  _tft->print(newStr);
}


void IRConfigs::drawMenu(const char text[][STRING_LENGTH], int nItems){
  int i;
  _tft->setTextSize(2);
  _tft->fillScreen(ST7735_BLACK);
  _tft->setCursor(0, 0);
  for (i = 0; i < nItems; i++){
    if (i == pos){
      _tft->setTextColor(ST7735_WHITE, ST7735_RED);
    } else {
      _tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
    }
    _tft->println(text[i]);
  }
}
void IRConfigs::leaveMenus(){
  screen = DUMMY;
  *lockScreen = false;
  _tft->fillScreen(ST7735_BLACK);
}

void IRConfigs::run(){
  int position, out;
  
  if (screen != DUMMY){
    if (millis()-lastTimePressed > WAITING_TIME){
      leaveMenus();
    }
  }
  if (irmp_get_data (&irmp_data)) {
      if (! (irmp_data.flags & IRMP_FLAG_REPETITION)) {
        // Update last time pressed
        lastTimePressed = millis();
        *lockScreen = true;

        // Screen secuence
        switch (screen) {
          case DUMMY:
            if (irmp_data.command == POWER_BTN){
              // Next State Logic
              screen = MAIN;
              setPos(mainText,MAIN_ITEMS,0);
            }
            break;
          case MAIN:
            // Current State Logic
            switch (irmp_data.command) {
              case DOWN_BTN:
                setPos(mainText,MAIN_ITEMS,(pos+1)%MAIN_ITEMS);
                break;
              case UP_BTN:
                position = pos-1;
                if (position < 0){
                  position = MAIN_ITEMS - 1;
                }
                setPos(mainText,MAIN_ITEMS,position);
                break;
              case OK_BTN:
                // Next State Logic
                switch (pos) {
                  case 0:  // Enter Pump Menu                  
                    screen = PUMP;
                    setPos(pumpText,PUMP_ITEMS,0);
                    break;
                  default:
                    break;
                }
                break;
              case LEFT_BTN:
              case POWER_BTN:
                leaveMenus();
                break;
              default:
                break;
            }
            break;
          case PUMP:
          // Current State Logic
            switch (irmp_data.command) {
              case DOWN_BTN:
                setPos(pumpText,PUMP_ITEMS,(pos+1)%PUMP_ITEMS);
                break;
              case UP_BTN:
                position = pos-1;
                if (position < 0){
                  position = PUMP_ITEMS - 1;
                }
                setPos(pumpText,PUMP_ITEMS,position);
                break;
              case OK_BTN:
                // Next State Logic
                switch (pos) {
                  case 0:  // Refill the upper tank                  
                    //TODO perform refill
                    leaveMenus();
                    break;
                  case 1:  // Configure Pump                   
                    screen = PUMP_CFG;
                    setPos(pumpCfgText,PUMP_CFG_ITEMS,0);
                    break;
                  default:
                    break;
                }
                break;
              case LEFT_BTN:
                screen = MAIN;
                setPos(mainText,MAIN_ITEMS,0);
                break;
              case POWER_BTN:
                leaveMenus();
                break;
              default:
                break;
            }
            break;

          case PUMP_CFG:
          // Current State Logic
            switch (irmp_data.command) {
              case DOWN_BTN:
                setPos(pumpCfgText,PUMP_CFG_ITEMS,(pos+1)%PUMP_CFG_ITEMS);
                break;
              case UP_BTN:
                position = pos-1;
                if (position < 0){
                  position = PUMP_CFG_ITEMS - 1;
                }
                setPos(pumpCfgText,PUMP_CFG_ITEMS,position);
                break;
              case OK_BTN:
                // Next State Logic
                switch (pos) {
                  case 0:  // Configure Lower Tank                    
                    currentTank = &lowerTank;    
                    screen = TANK_CFG;
                    setPos(tankCfgText,TANK_CFG_ITEMS,0);
                    drawTankParameters();
                    break;
                  case 1:  // Configure Upper Tank  
                    currentTank = &upperTank;
                    screen = TANK_CFG;
                    setPos(tankCfgText,TANK_CFG_ITEMS,0);
                    drawTankParameters();
                    break;
                  default:
                    break;
                }
                break;
              case POWER_BTN:
                leaveMenus();
                break;
              case LEFT_BTN:
                screen = PUMP;
                setPos(pumpText,PUMP_ITEMS,0);
                break;
              default:
                break;
            }
            break;

          case TANK_CFG:
          // Current State Logic
            switch (irmp_data.command) {
              case DOWN_BTN:
                setPos(tankCfgText,TANK_CFG_ITEMS,(pos+1)%TANK_CFG_ITEMS);
                drawTankParameters();
                break;
              case UP_BTN:
                position = pos-1;
                if (position < 0){
                  position = TANK_CFG_ITEMS - 1;
                }
                setPos(tankCfgText,TANK_CFG_ITEMS,position);
                drawTankParameters();
                break;
              case OK_BTN:
                // Next State Logic
                screen = EDIT;
                prepareEditScreen();
                break;
              case POWER_BTN:
                leaveMenus();
                break;
              case LEFT_BTN:
                screen = PUMP_CFG;
                setPos(pumpCfgText,PUMP_CFG_ITEMS,0);
                break;
              default:
                break;
            }
            break;

          case EDIT:
          // Current State Logic
            switch (irmp_data.command) {              
              case OK_BTN:
                switch (pos){
                  case 0: // Height
                    out = currentTank->setHeight(newStr.toInt());
                    break;
                  case 1: // Gap
                    out = currentTank->setGap(newStr.toInt());
                    break;
                  case 2: // Restart
                    out = currentTank->setRestart(newStr.toInt());
                    break;
                  case 3: // Min
                    out = currentTank->setMin(newStr.toInt());
                    break;
                  default:
                    break;
                }
                // Guardar el valor si es correcto e ir al menu anterior
                if (out == 0) {
                  screen = TANK_CFG;
                  setPos(tankCfgText,TANK_CFG_ITEMS,0);
                  drawTankParameters();                  
                } else {
                  printParameterValue(ST7735_RED);
                }
                break;
              case POWER_BTN:
                leaveMenus();
                break;
              case LEFT_BTN:
                // Delete the last character if there is any
                if (newStr.length() > 0){
                  newStr = newStr.substring(0,newStr.length()-1);
                  printParameterValue(ST7735_BLACK);
                } else { // Go back to previous screen if empty data
                    screen = TANK_CFG;
                    setPos(tankCfgText,TANK_CFG_ITEMS,0);
                    drawTankParameters();
                }
                break;
              default: // Any number
                if (newStr.length() < MAX_INPUT_LENGTH){                
                  if (irmp_data.command >= 0 && irmp_data.command <= 9){
                    newStr += irmp_data.command;
                    printParameterValue(ST7735_BLACK);
                  }
                }
                break;
            }
            break;

          default:
            break;
          }
        }
    }
}
