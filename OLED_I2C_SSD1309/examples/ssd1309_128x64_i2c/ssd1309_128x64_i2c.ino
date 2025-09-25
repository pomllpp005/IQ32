/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1309 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <OLED_I2C_SSD1309.h>

#define OLED_RESET -1
OLED_I2C_SSD1309 oled(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1309_LCDHEIGHT != 64)
#error("Height incorrect, please fix OLED_I2C_SSD1309.h!");
#endif

void setup()   {                
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  oled.begin(SSD1309_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  oled.text(0,0,"Test %d",12345);
  oled.display();
  oled.textSize(2);
  oled.text(2,0,"Line %h",0xabcd);
  oled.display();
  delay(2000);

  // Clear the buffer.
  oled.clearDisplay();

  // draw a single pixel
  oled.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // draw many lines
  testdrawline();
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // draw rectangles
  testdrawrect();
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // draw a white circle, 10 pixel radius
  oled.fillCircle(oled.width()/2, oled.height()/2, 10, WHITE);
  oled.display();
  delay(2000);
  oled.clearDisplay();

  testdrawroundrect();
  delay(2000);
  oled.clearDisplay();

  testfillroundrect();
  delay(2000);
  oled.clearDisplay();

  testdrawtriangle();
  delay(2000);
  oled.clearDisplay();
   
  testfilltriangle();
  delay(2000);
  oled.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  oled.clearDisplay();

  // text display tests
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.println("Hello, world!");
  oled.setTextColor(BLACK, WHITE); // 'inverted' text
  oled.println(3.141592);
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.print("0x");
  oled.println(0xDEADBEEF, HEX);
  oled.display();
  delay(2000);
  oled.clearDisplay();

  // miniature bitmap display
  oled.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  oled.display();
  delay(1);

  // invert the display
  oled.invertDisplay(true);
  delay(1000); 
  oled.invertDisplay(false);
  delay(1000); 
  oled.clearDisplay();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}


void loop() {
  
}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(oled.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oled.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    oled.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      oled.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > oled.height()) {
        icons[f][XPOS] = random(oled.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    oled.write(i);
    if ((i > 0) && (i % 21 == 0))
      oled.println();
  }    
  oled.display();
  delay(1);
}

void testdrawcircle(void) {
  for (int16_t i=0; i<oled.height(); i+=2) {
    oled.drawCircle(oled.width()/2, oled.height()/2, i, WHITE);
    oled.display();
    delay(1);
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<oled.height()/2; i+=3) {
    // alternate colors
    oled.fillRect(i, i, oled.width()-i*2, oled.height()-i*2, color%2);
    oled.display();
    delay(1);
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(oled.width(),oled.height())/2; i+=5) {
    oled.drawTriangle(oled.width()/2, oled.height()/2-i,
                     oled.width()/2-i, oled.height()/2+i,
                     oled.width()/2+i, oled.height()/2+i, WHITE);
    oled.display();
    delay(1);
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(oled.width(),oled.height())/2; i>0; i-=5) {
    oled.fillTriangle(oled.width()/2, oled.height()/2-i,
                     oled.width()/2-i, oled.height()/2+i,
                     oled.width()/2+i, oled.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oled.display();
    delay(1);
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<oled.height()/2-2; i+=2) {
    oled.drawRoundRect(i, i, oled.width()-2*i, oled.height()-2*i, oled.height()/4, WHITE);
    oled.display();
    delay(1);
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<oled.height()/2-2; i+=2) {
    oled.fillRoundRect(i, i, oled.width()-2*i, oled.height()-2*i, oled.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oled.display();
    delay(1);
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<oled.height()/2; i+=2) {
    oled.drawRect(i, i, oled.width()-2*i, oled.height()-2*i, WHITE);
    oled.display();
    delay(1);
  }
}

void testdrawline() {  
  for (int16_t i=0; i<oled.width(); i+=4) {
    oled.drawLine(0, 0, i, oled.height()-1, WHITE);
    oled.display();
    delay(1);
  }
  for (int16_t i=0; i<oled.height(); i+=4) {
    oled.drawLine(0, 0, oled.width()-1, i, WHITE);
    oled.display();
    delay(1);
  }
  delay(250);
  
  oled.clearDisplay();
  for (int16_t i=0; i<oled.width(); i+=4) {
    oled.drawLine(0, oled.height()-1, i, 0, WHITE);
    oled.display();
    delay(1);
  }
  for (int16_t i=oled.height()-1; i>=0; i-=4) {
    oled.drawLine(0, oled.height()-1, oled.width()-1, i, WHITE);
    oled.display();
    delay(1);
  }
  delay(250);
  
  oled.clearDisplay();
  for (int16_t i=oled.width()-1; i>=0; i-=4) {
    oled.drawLine(oled.width()-1, oled.height()-1, i, 0, WHITE);
    oled.display();
    delay(1);
  }
  for (int16_t i=oled.height()-1; i>=0; i-=4) {
    oled.drawLine(oled.width()-1, oled.height()-1, 0, i, WHITE);
    oled.display();
    delay(1);
  }
  delay(250);

  oled.clearDisplay();
  for (int16_t i=0; i<oled.height(); i+=4) {
    oled.drawLine(oled.width()-1, 0, 0, i, WHITE);
    oled.display();
    delay(1);
  }
  for (int16_t i=0; i<oled.width(); i+=4) {
    oled.drawLine(oled.width()-1, 0, i, oled.height()-1, WHITE); 
    oled.display();
    delay(1);
  }
  delay(250);
}

void testscrolltext(void) {
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setCursor(10,0);
  oled.clearDisplay();
  oled.print("scroll");
  oled.display();
  delay(1);
 
  oled.startscrollright(0x00, 0x0F);
  delay(2000);
  oled.stopscroll();
  delay(1000);
  oled.startscrollleft(0x00, 0x0F);
  delay(2000);
  oled.stopscroll();
  delay(1000);    
  oled.startscrolldiagright(0x00, 0x07);
  delay(2000);
  oled.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  oled.stopscroll();
}