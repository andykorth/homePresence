#include "Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "Adafruit_PN532.h"

// If using software SPI (the default case):
#define OLED_MOSI   2
#define OLED_CLK   3
#define OLED_DC    4
#define OLED_CS    5
#define OLED_RESET 6

#define TEMP_BUTTON A0

#define DEBUG

// name the pins
#define LED_1 D0
#define LED_2 D1
int count = 0;

int people = 0;

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define PERSON_COUNT 5

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
	int state;
};

struct User Users[PERSON_COUNT] = {
	{"Captain Faceman", {0x27, 0xBD, 0x77, 0x79}, 0},
	{"Jorge", {0x24, 0xB1, 0x41, 0xF4}, 0},
	{"The Stickler", {0x97, 0x2C, 0x11, 0xCB}, 0},
	{"Cardla", {0xC3, 0x71, 0x26, 0xD0}, 0},
	{"Baby Bertha", {0x47, 0x91, 0x33, 0x21}, 0},
};

int currentNote = -1;
bool toggled = true;
int delayUntil = 0;
void (* currentFunction)(void);

// This routine runs only once upon reset
void setup()
{
	//Register our Spark function here
	Spark.function("led", ledControl);
	
	// Configure the pins to be outputs
	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
	pinMode(D7, OUTPUT);
	
	pinMode(TEMP_BUTTON, INPUT);
	
	// Initialize both the LEDs to be OFF
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	
	//Setting up automatically synced variables with spark backend
	Spark.variable("People", &people, INT);
	
	// setup display:
	Serial.begin(9600);
	
	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC);
	// init done
	
	// Setup the RFID reader.
	SetupNFC();
	
	currentFunction = PleaseSwipeTag;
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

static void Debug(char *message){
#ifdef DEBUG
	display.setTextSize(1);
	display.setTextColor(WHITE, BLACK);
	
	display.setCursor(0, 56);
	display.print(message);
	
	display.display();
#endif
}

static void Goodbye(char *name){
	display.clearDisplay();
	
	display.setTextSize(1);
	display.setTextColor(WHITE);
	
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
		people = people ^ (1 << personIndex);
		
		if( people & (1 << personIndex )) {
			Welcome(Users[personIndex].name);
		}else{
			Goodbye(Users[personIndex].name);
		}
	}
}

// Returns the user's index or -1 if no user.
static int PollNFC(void){
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
	
	if(!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)){
		Debug("Read Error.");
		return -1;
	}
	
	if(uidLength != UID_LENGTH){
		Debug("Bad UID.");
		return -1;
	}
	
	int count = sizeof(Users)/sizeof(*Users);
	for(int i=0; i<count; i++){
		if(memcmp(uid, Users[i].uid, UID_LENGTH) == 0){
			return i;
		}
	}
	
	return -1;
}

// This routine loops forever
void loop()
{
	if(millis() < delayUntil){
		return;
	}else{
		currentFunction();
	}
}

void PleaseSwipeTag(void){
	Welcome("Please Swipe Your Tag");
	currentFunction = CheckNFC;
}

void CheckNFC(void){
	Debug("Polling");
	
	int user_index = PollNFC();
	if(user_index != -1){
		Welcome(Users[user_index].name);
		TogglePersonState(user_index);
		delayUntil = millis() + 4000;
		currentFunction = PleaseSwipeTag;
	} else {
		Debug("---No One---");
	}
	
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