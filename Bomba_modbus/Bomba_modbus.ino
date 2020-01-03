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

// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//bool LOCK_SCREEN = false;

PumpController pump = PumpController();

PumpHMI hmi = PumpHMI(&tft);

#define TXEN PB12
Modbus slave(1, 3, TXEN); // this is slave @1 and RS-485
MemoryHandler MBmemoryHandler = MemoryHandler(&pump);

// // data array for modbus network sharing
// uint16_t au16data[16] = {
//     3, 1415, 9265, 4, 2, 7182, 28182, 8, 0, 0, 0, 0, 0, 0, 1, -1};

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

  // Setup HMI configurations
  hmi.drawHMItemplate(pump.getLowerTankMin(), pump.getUpperTankMin());

  // ----------------------- Register Tasks ---------------------------
  xTaskCreate(vPumpControlTask,
              "TaskPumpControl",
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
  // // int level, count = 0;

  // if ((millis() - lastPrint) > 1000)
  // {
  //   //simulate();
  //   //hmi.animate();
  //   if (!LOCK_SCREEN)
  //   {
  //     hmi.updateLowerTankLevel(pump.getLowerTankLevel());
  //     hmi.updateUpperTankLevel(pump.getUpperTankLevel());
  //   }
  //   // // Print the distance
  //   // tft.setCursor(70, 40);
  //   // char tbs[6];
  //   // sprintf(tbs, "%3dmm", pump.getUpperTankLevel());
  //   // tft.println(tbs);
  //   lastPrint = millis();
  // }
  // pump.run();
}

// void simulate()
// {
//   hmi.updateLowerTankLevel(levelDown);
//   hmi.updateUpperTankLevel(levelUp);
//   levelUp = (levelUp + 5) % 100;
//   levelDown = 100 - levelUp;
// }
