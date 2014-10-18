#include "Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "Adafruit_PN532.h"
#include "notes.h"

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

#definte PERSON_COUNT 5

#define NFC_SCK  (A2)
#define NFC_MOSI (A3)
#define NFC_SS   (A4)
#define NFC_MISO (A5)

Adafruit_PN532 nfc(NFC_SCK, NFC_MISO, NFC_MOSI, NFC_SS);

static void SetupNFC(void){
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Welcome("No NFC.");
    while (1); // halt
  }

  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Welcome("NFC");
}

#define UID_LENGTH 4
struct User {
//  uint8_t uid[4];
  char *name;
  uint8_t uid[UID_LENGTH];
};

struct User Users[5] = {
  {"Captain Faceman", {0x27, 0xBD, 0x77, 0x79}},
  {"Jorge", {0x24, 0xB1, 0x41, 0xF4}},
  {"The Stickler", {0x97, 0x2C, 0x11, 0xCB}},
  {"Cardla", {0xC3, 0x71, 0x26, 0xD0}},
  {"Baby Bertha", {0x47, 0x91, 0x33, 0x21}},
};

// State of our users:
int[PERSON_COUNT] peopleStatus;

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
	
	pinMode(TEMP_BUTTON, INPUT);
	
	// Initialize both the LEDs to be OFF
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	
	//Setting up automatically synced variables with spark backend
	Spark.variable("count",						&count, INT);
	Spark.variable("CaptainFaceman",	&peopleStatus[0], INT);
	Spark.variable("Jorge",						&peopleStatus[1], INT);
	Spark.variable("TheStickler",			&peopleStatus[2], INT);
	Spark.variable("Cardla",					&peopleStatus[3], INT);
	Spark.variable("BabyBertha",			&peopleStatus[4], INT);
	
	// setup display:
	Serial.begin(9600);
	
	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC);
	// init done
	
	// Setup the RFID reader.
	SetupNFC();
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

static void Goodbye(char *name){
	display.clearDisplay();
	
	display.setTextSize(1);
	display.setTextColor(RED);
	
	display.setCursor(25, 25);
	display.println("Goodbye Friend!");
	
	int width = 6*strlen(name) - 1;
	display.setCursor(64 - width/2, 33);
	display.println(name);
	
	display.display();
}

void TogglePersonState(int personIndex)
{
	if(personIndex < 0 || personIndex > PERSON_COUNT)
	{
		Goodbye("Bad person.");
	}else{
		peopleStatus[personIndex] = !peopleStatus[personIndex];
	}
}

int currentNote = -1;
bool toggled = true;
// This routine loops forever
void loop()
{
	
	if(	digitalRead(TEMP_BUTTON) == HIGH && toggled){
		jorge = !jorge;
		toggled = false;
		
		if(jorge){
			Welcome("JORGE AWAY");
		}else{
			Welcome("JORGE HOME");
		}
	}else{
		toggled = true;
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