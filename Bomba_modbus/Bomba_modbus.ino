/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <MapleFreeRTOS900.h>
#include "hmi.h"
#include "pump_controller.h"
#include "ModbusRtu.h"
#include "MB_memory_handler.h"
#include "configs.h"
#include "LightController.h"

// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Lights
LightController lamp = LightController();

PumpController pump = PumpController();

PumpHMI hmi = PumpHMI(&tft);

#define TXEN PB12
Modbus slave(1, 3, TXEN); // this is slave @1 and RS-485
MemoryHandler MBmemoryHandler = MemoryHandler(&pump, &lamp);

// // For simulation
// int levelUp;
// int levelDown;
// unsigned int lastPrint = 0;

// // ------------------------- Tasks -----------------------------
static void vPumpControlTask(void *pvParameters)
{
  // Execute task every 100ms
  int xTicks100ms = pdMS_TO_TICKS(100);
  for (;;)
  {
    pump.run();
    vTaskDelay(xTicks100ms);
  }
}

static void vLightControlTask(void *pvParameters)
{
  // Execute task every 100ms
  int xTicks100ms = pdMS_TO_TICKS(100);
  for (;;)
  {
    lamp.run();
    vTaskDelay(xTicks100ms);
  }
}

static void vModbusTask(void *pvParameters)
{
  for (;;)
  {
    slave.poll(&MBmemoryHandler);
  }
}

static void vUpdateScreenTask(void *pvParameters)
{
  // Execute task every 1s
  int xTicks1s = pdMS_TO_TICKS(1000);
  for (;;)
  {
    hmi.updateLowerTankLevel(pump.getLowerTankLevel());
    hmi.updateUpperTankLevel(pump.getUpperTankLevel());
    vTaskDelay(xTicks1s);
  }
}

void setup(void)
{
  // ---------------- Initial setup --------------------------------
  // EEPROM Memory
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize = 0x400;

  // Modbus RTU config
  slave.begin(MB_SPEED); // baud-rate at 19200

  // Use this initializer if you're using a 1.8" TFT
  SPI.begin();
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab

  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(0);

  // Setup text
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

  // Setup Pump Controller configurations
  pump.init();

  // Setup Lights
  lamp.init(LightPIN0, pirPIN, ldrPIN, ADR_LIGHT_MODE_0, ADR_LIGHT_SLEEP_TIME_0,
            ADR_LIGHT_SMART_0, ADR_LIGHT_SMART_DELAY_0, ADR_LIGHT_INIT_DELAY_0,
            ADR_LIGHT_DELAY_INCREMENT_0, ADR_LIGHT_TRESHOLD_0);

  // Setup HMI configurations
  hmi.drawHMItemplate(pump.getLowerTankMin(), pump.getUpperTankMin());

  // ----------------------- Register Tasks ---------------------------
  xTaskCreate(vPumpControlTask,
              "TaskPumpControl",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              NULL);

  xTaskCreate(vLightControlTask,
              "TaskLightControl",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              NULL);

  xTaskCreate(vUpdateScreenTask,
              "TaskScreenUpdate",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              NULL);

  xTaskCreate(vModbusTask,
              "TaskModbus",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY,
              NULL);

  vTaskStartScheduler();
}

void loop()
{

}

// void simulate()
// {
//   hmi.updateLowerTankLevel(levelDown);
//   hmi.updateUpperTankLevel(levelUp);
//   levelUp = (levelUp + 5) % 100;
//   levelDown = 100 - levelUp;
// }
