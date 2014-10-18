// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_SSD1306/Adafruit_SSD1306.h"


// If using software SPI (the default case):
#define OLED_MOSI   2
#define OLED_CLK   3
#define OLED_DC    4
#define OLED_CS    5
#define OLED_RESET 6

#define TEMP_BUTTON A0

// name the pins
#define LED_1 D0
#define LED_2 D1
int count = 0;

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define TEXT_WIDTH 21

// State of our users:
int jorge = 0;


// This routine runs only once upon reset
void setup()
{
	//Register our Spark function here
	Spark.function("led", ledControl);
	Spark.function("resetCounter", resetCounterFunc);
	
	// Configure the pins to be outputs
	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
	
	// Initialize both the LEDs to be OFF
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	Spark.variable("count", &count, INT);


	Spark.variable("Jorge", &jorge, INT);

	
	pinMode(TEMP_BUTTON, INPUT);
	
	// setup display:
	Serial.begin(9600);
	
	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC);
	// init done
	
	Welcome("Captain Faceman");
	
	
}


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



// This routine loops forever
void loop()
{
	if(	digitalRead(TEMP_BUTTON) == HIGH){
		jorge = !jorge;
		
	}
	
	// Nothing to do here
}


int resetCounterFunc(String command)
{
	count = 0;
	return 1;
}


int ledControl(String command)
{
	int state = 0;
	//find out the pin number and convert the ascii to integer
	int pinNumber = (command.charAt(1) - '0') - 1;
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber < 0 || pinNumber > 1) return -1;
	
	// find out the state of the led
	if(command.substring(3,7) == "HIGH") state = 1;
	else if(command.substring(3,6) == "LOW") state = 0;
	else return -1;
	
	// write to the appropriate pin
	digitalWrite(pinNumber, state);
	count = count+1;
	return 1;
}