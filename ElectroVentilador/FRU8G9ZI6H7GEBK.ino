/*
 Show all the fonts and demonstrate wrap function.
 Only font sizes 1, 2, 4, 6 and 7 are implemented in the Adafruit_GFX_AS library.
 */

#define sclk 13  // Don't change
#define mosi 11  // Don't change
#define cs   10
#define dc   9
#define rst  7  // you can also connect this to the Arduino reset

#include <Adafruit_GFX_AS.h>    // Core graphics library
#include <Adafruit_ILI9341_AS.h> // Hardware-specific library
#include <SPI.h>

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(cs, dc, rst);       // Invoke custom library

// ###############  Add these lines #############
#ifdef LOAD_GLCD
  #include "glcdfont.c"
#endif

#ifdef LOAD_FONT2
  #include "Font16.h"
#endif

#ifdef LOAD_FONT4
#include "Font32.h"
#endif

#ifdef LOAD_FONT6
#include "Font64.h"
#endif

#ifdef LOAD_FONT7
  #include "Font7s.h"
#endif
// ##############################################

unsigned long targetTime = 0;
byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11;

void setup(void) {
  tft.init();
  tft.setRotation(1);
}

void loop() {

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

  wrapString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, 0, 1, 1);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, 0, 2, 1);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, 0, 4, 1);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" -.0123456789:amp", 0, 0, 6, 1);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" -.0123456789:", 0, 0, 7, 1);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, 0, 1, 2);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, 0, 2, 2);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, 0, 4, 2);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" -.012p3456789:amp", 0, 0, 6, 2);

  delay(4000);

  tft.fillScreen(ILI9341_BLACK);
  wrapString(" -.0123456789:", 0, 0, 7, 2);
  
  delay(4000);
}

// Add this function to your sketch, it will draw the character string
// on the display and wrap to next line if too long.
//
// dX and dY = top left plot coordinates
// font = font number
// text size is multiplier for size, normally 1
void wrapString(char *string, int dX, int dY, int font, int textsize)
{
  int len = 0;
  int hgt = 0;
  char ascii;
  int width = tft.width();
  int height = tft.height();
  
  while (*string)
  {
    ascii = *string;

#ifdef LOAD_GLCD
    if (font == 1) {
      len = 6 * textsize;
      hgt = 8 * textsize;
    }
#endif

#ifdef LOAD_FONT2
    if (font == 2) {
      len = (pgm_read_byte(widtbl_f16 + ascii - 32) + 1) * textsize;
      hgt = 16 * textsize;
    }
#endif

#ifdef LOAD_FONT4
    if (font == 4) {
      len = (pgm_read_byte(widtbl_f32 + ascii - 32) - 3) * textsize;
      hgt = 24 * textsize;
    }
#endif

#ifdef LOAD_FONT6
    if (font == 6) {
      len = (pgm_read_byte(widtbl_f64 + ascii - 32) - 3) * textsize;
      hgt = 48 * textsize;
    }
#endif

#ifdef LOAD_FONT7
    if (font == 7) {
      len = (pgm_read_byte(widtbl_f7s + ascii - 32) + 2) * textsize;
      hgt = 52 * textsize;
    }
#endif

    if (dX + len > width) {
      dX = 0; dY += hgt;
      if (dY + hgt > height) dY = 0; // Wrap to top of screen but may not look good!
    }
    tft.setTextSize(textsize);
    tft.drawChar(*string, dX, dY, font);
    dX+=len;
    *string++;
  }
}




