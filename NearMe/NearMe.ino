#include <Adafruit_PN532.h>

#define SCK  (2)
#define MOSI (3)
#define SS   (4)
#define MISO (5)

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);

static void SetupNFC(void){
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
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

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");
  
  SetupNFC();
}

// Returns the user's index or -1 if no user.
static int PollNFC(void){
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  if(!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)){
    return -1;
  }
  
  if(uidLength != UID_LENGTH){
    Serial.println("Wrong type of RFID");
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

void loop(void) {
  int user_index = PollNFC();
  if(user_index != -1){
    Serial.print("Welcome home ");
    Serial.print(Users[user_index].name);
    Serial.println(".");
    
    delay(2000);
  }
}

