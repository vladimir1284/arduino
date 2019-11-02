
#include "All_fonts.h"
/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS PA4
#define TFT_RST PA2 // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC PA3

// Option 1 (recommended): must use the hardware SPI pins
// TFT_SCLK PA5   // SPI 1
// TFT_MOSI PA7   // SPI 1
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Colours
#define BLACK   0x0000
#define WHITE   0xFFFF

#define TEXT "aA MWxyz 1234" // Text that will be printed on screen in any font

#define SHOWFONT(f) showFont(&f, #f) // Simplify parameter passing

void setup(void)
{
  // >>>>>>>>>> Initialise the driver here:
  tft.begin();
  
  tft.setRotation(1);
}

void loop()
{
  // Remove the // from any line below to display text in that font
  // the number of fonts that can be tested will be limited by flash space
  //
  // As supplied ff1 and ff48 will be displayed
  //
  // Typically an UNO can load 4 fonts but realistically would need to load
  // just one or two to leave space for other sketch code

  SHOWFONT(ff1);
  //SHOWFONT(ff2);
  //SHOWFONT(ff3);
  //SHOWFONT(ff4);

  //SHOWFONT(ff5);
  //SHOWFONT(ff6);
  //SHOWFONT(ff7);
  //SHOWFONT(ff8);

  //SHOWFONT(ff9);
  //SHOWFONT(ff10);
  //SHOWFONT(ff11);
  //SHOWFONT(ff12);

  //SHOWFONT(ff13);
  //SHOWFONT(ff14);
  //SHOWFONT(ff15);
  //SHOWFONT(ff16);

  //SHOWFONT(ff17);
  //SHOWFONT(ff18);
  //SHOWFONT(ff19);
  //SHOWFONT(ff20);

  //SHOWFONT(ff21);
  //SHOWFONT(ff22);
  //SHOWFONT(ff23);
  //SHOWFONT(ff24);

  //SHOWFONT(ff25);
  //SHOWFONT(ff26);
  //SHOWFONT(ff27);
  //SHOWFONT(ff28);

  //SHOWFONT(ff29);
  //SHOWFONT(ff30);
  //SHOWFONT(ff31);
  //SHOWFONT(ff32);

  //SHOWFONT(ff33);
  //SHOWFONT(ff34);
  //SHOWFONT(ff35);
  //SHOWFONT(ff36);

  //SHOWFONT(ff37);
  //SHOWFONT(ff38);
  //SHOWFONT(ff39);
  //SHOWFONT(ff40);

  //SHOWFONT(ff41);
  //SHOWFONT(ff42);
  //SHOWFONT(ff43);
  //SHOWFONT(ff44);

  //SHOWFONT(ff45);
  //SHOWFONT(ff46);
  //SHOWFONT(ff47);
  SHOWFONT(ff48);
}

void showFont(const GFXfont *font, char *name)
{
  // Clear the screen, set cursor to top left, set text size multiplier to 1
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(1);

  // Note a limitation of the published Adafruit_GFX library is that it will not draw the background
  // even if we try to specify this!
  tft.setTextColor(WHITE, BLACK);

  // Print with original GLCD font for comparison
  tft.setFont(ff0); // Use (ff0) (NULL) or () to call up GLCD font
  tft.println("Font: " + String(name));
  tft.println(TEXT);
  tft.println(); // This moves the cursor down the font height, needed as text plot datum is
                 // now bottom left (was top left for original GLCD font)
                 
  // Now print with new fonts
  tft.setFont(font);
  tft.println(); // This moves the cursor down the font height, needed as text plot datum is
                 // now bottom left (was top left for original GLCD font)

  tft.println("Font: " + String(name));
  tft.println(TEXT);

  delay(2000);
}

