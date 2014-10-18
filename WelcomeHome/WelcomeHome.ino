#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define TEXT_WIDTH 21

static void Welcome(char *name){
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(25, 25);
  display.println("Welcome home!");
  
  int width = 6*strlen(name) - 1;
  display.setCursor(64 - width/2, 33);
  display.println(name);
  
  display.display();
}

void setup()   {                
  Serial.begin(9600);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done
  
  Welcome("Captain Faceman");
}

void loop(){}

