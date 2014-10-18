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

//music
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// notes in the melody:
int melody[] = {
	NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
	4, 8, 8, 4,4,4,4,4 };


// This routine runs only once upon reset
void setup()
{
	//Register our Spark function here
	Spark.function("led", ledControl);
	Spark.function("resetCounter", resetCounterFunc);
	
	// Configure the pins to be outputs
	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
	pinMode(D7, OUTPUT);
	
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


int debounce = true;
int currentNote = -1;
// This routine loops forever
void loop()
{
	if(	digitalRead(TEMP_BUTTON) == HIGH && debounce){
		debounce = false;
		jorge = !jorge;
	}else{
		debounce = true;
	}
	
	/*
	 if(currentNote != -1){
	 
	 char* str = "note: x";
	 str[6] = currentNote;
	 Welcome(str);
	 
		// to calculate the note duration, take one second
		// divided by the note type.
		//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
		int noteDuration = 1000/noteDurations[currentNote];
		tone(D7, melody[currentNote],noteDuration);
		
		// to distinguish the notes, set a minimum time between them.
		// the note's duration + 30% seems to work well:
		int pauseBetweenNotes = noteDuration * 1.30;
		delay(pauseBetweenNotes);
		// stop the tone playing:
		noTone(D7);
		currentNote = (currentNote+1);
		if(currentNote > 8){
	 currentNote = -1;
		}
	 }
	 */
	
	// Nothing to do here
}


int resetCounterFunc(String command)
{
	count = 0;
	currentNote = 0;
	Welcome("Reset Button");
	
	return 1;
}


int ledControl(String command)
{
	int state = 0;
	//find out the pin number and convert the ascii to integer
	int pinNumber = (command.charAt(1) - '0') - 1;
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber < 0 || pinNumber > 1) return -1;
	
	
	if(pinNumber == 0){
		Welcome("Red LED");
	}else{
		Welcome("Green LED");
	}
	
	
	
	// find out the state of the led
	if(command.substring(3,7) == "HIGH") state = 1;
	else if(command.substring(3,6) == "LOW") state = 0;
	else return -1;
	
	// write to the appropriate pin
	digitalWrite(pinNumber, state);
	count = count+1;
	return 1;
}