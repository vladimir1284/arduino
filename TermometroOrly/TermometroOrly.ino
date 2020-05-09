#include "max6675.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// If using software SPI (the default case):
#define OLED_MOSI PB6
#define OLED_CLK PB7
#define OLED_DC PB8
#define OLED_CS PA15
#define OLED_RESET PB9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define THERM_DO PA8
#define THERM_CS PA9
#define THERM_CLK PA10
MAX6675 thermocouple(THERM_CLK, THERM_CS, THERM_DO);

#define SENSOR_DELAY 100  // ms
#define SPEED_INTERVAL 50 // sample periods (10seg)
#define SPEED_FACTOR 6

char tbs[3];
float lastTemp;
int count;

// filter
#define SIZE 15
int head;
float temps[SIZE];
float averageTemp = 0;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC);

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);

  count = 0;
  head = 0;
  lastTemp = thermocouple.readCelsius();

  // Celcious degrees
  display.setTextSize(3);
  display.setCursor(80, 6);
  display.print((char)247);
  display.print("C");
  // drawing commands to make them visible on screen!
  display.display();
}

void loop()
{
  float temp = filter(thermocouple.readCelsius());
  delay(SENSOR_DELAY);

  display.setCursor(0, 0);
  display.setTextSize(4);
  sprintf(tbs, "%3d", int(temp + 0.5));
  display.print(tbs);
  // drawing commands to make them visible on screen!
  display.display();
  delay(SENSOR_DELAY);

  count = (count + 1) % SPEED_INTERVAL;
  if (count == 0)
  {
    float speed = (temp - lastTemp) * SPEED_FACTOR;
    lastTemp = temp;
    display.setTextSize(2);
    display.setCursor(0, 40);
    sprintf(tbs, "%5.1f", speed);
    display.print(tbs);
    display.print((char)247);
    display.print("C/m");
    // drawing commands to make them visible on screen!
    display.display();
  }
}

float filter(float currentValue)
{
  averageTemp -= temps[head]/SIZE;
  averageTemp += currentValue/SIZE;
  temps[head] = currentValue;
  head = (head+1) % SIZE;
}