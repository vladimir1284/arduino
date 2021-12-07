#include <TTVout.h>
#include <fontALL.h>
#include "hmi.h"
#include "configs.h"
#include "controller.h"

TTVout TV;

RPM rpm_meter = RPM();

GPS gps_controller = GPS(&Serial3);

unsigned int lastUpdated = 0;
int speed, temp;

// Input pin for the button
Button btn = Button();

int screenTask = WORK;

// Simulator
bool testPrepared = false;

// Controller
ElectroController controller = ElectroController();

// Configurations
Configs cfgs = Configs(&TV, &screenTask, &btn);

// HMI
ThermHMI hmi = ThermHMI(&TV, &gps_controller, &rpm_meter, &screenTask, &cfgs, &controller);

void setup(void)
{
  TV.adjust(0,       // 垂直同期補正
           -9,       // 横表示開始補正
            8        // 縦表示開始補正
            );
  TV.select_font(font6x8);

  TV.begin(SC_224x108);  
  //TV.begin(SC_448x216);  

  screenTask = WORK;

  // Setups
  btn.init(BTN_PIN);
  cfgs.init();
  rpm_meter.init(PIN_RPM);

  // Init the controller
  controller.init(PIN_NTC, PIN_BUZ, PIN_FAN0, PIN_FAN1,
                  cfgs.getTemp(), cfgs.getHyst(),
                  cfgs.getCalibration(),cfgs.getVoltCalibration(),
                  PIN_A, PIN_B, PIN_VOLT, PIN_BUZ_ACT);

  // Buzzer sound indicating ready
  int i;
  for (i = 0; i < 2; i++)
  {
    controller.turnOnBuzzer();
    delay(INIT_BUZZ_DELAY);
    controller.turnOffBuzzer();
    delay(INIT_BUZZ_DELAY / 2);
  }
  // Init screen
    if (screenTask == WORK)
  {
    hmi.drawHMItemplate();
    hmi.showAll();
    delay(INIT_SCREEN_DELAY);
  }
}

void loop()
{
  rpm_meter.run();
  gps_controller.run();
  btn.run();
  cfgs.run();
  if (screenTask != TEST)
  {
    controller.run();
    hmi.update();
  }
  else
  {
    controller.simulate();
    hmi.update();
  }
  verifyConfigChanges();
}

void verifyConfigChanges()
{
  if (screenTask == PENDING)
  {
    controller.setTemp1(cfgs.getTemp());
    controller.setHysteresis(cfgs.getHyst());
    controller.setCalibration(cfgs.getCalibration());
    controller.setVoltCalibration(cfgs.getVoltCalibration());

    hmi.drawHMItemplate();

    screenTask = WORK;
  }
  if (screenTask == TEST && !testPrepared)
  {
    controller.setTemp1(cfgs.getTemp());
    controller.setHysteresis(cfgs.getHyst());
    controller.setCalibration(cfgs.getCalibration());
    controller.setVoltCalibration(cfgs.getVoltCalibration());

    hmi.drawHMItemplate();

    testPrepared = true;
  }
}
