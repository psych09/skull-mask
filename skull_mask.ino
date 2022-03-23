// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
#define CARDCS 4     // Card chip select pin

// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 2       // VS1053 Data request, ideally an Interrupt pin


#define RED_LED A5
#define GREEN_LED A4
#define BLUE_LED A3

#define JAW_TRIGGER_INPUT A0
#define NEXT_BUTTON_INPUT A1
#define BACK_BUTTON_INPUT A2


Adafruit_VS1053_FilePlayer musicPlayer = 
  // create shield object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

int jawTriggerState = 1;
int nextButtonState = 1;
int backButtonState = 1;



int index = 0;
String audioFiles[] = {"/ghstgasp.mp3", "/ghstgrwl.mp3", "/ghstmoan.mp3"};
int len = (sizeof(audioFiles) / sizeof(String));


////

void setup() {
  pinMode(JAW_TRIGGER_INPUT, INPUT_PULLUP);
  pinMode(NEXT_BUTTON_INPUT, INPUT_PULLUP);
  pinMode(BACK_BUTTON_INPUT, INPUT_PULLUP);
  
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  // printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}



void loop() {  
  jawTriggerState = digitalRead(JAW_TRIGGER_INPUT);
  nextButtonState = digitalRead(NEXT_BUTTON_INPUT);
  backButtonState = digitalRead(BACK_BUTTON_INPUT);
//  Serial.println(jawTriggerState);
//  Serial.println(nextButtonState);
//  Serial.println(backButtonState);
//  Serial.println(index);
  
 

  if(nextButtonState == 0)
  {
    if(index > 1)
    {
      index = 0;
      delay(500);
    }
    else
    {
      index += 1;
      delay(500); 
    }
  }

  if(backButtonState == 0)
  {
    if(index < 1)
    {
      index = 2;
      delay(500);
    }
    else
    {
      index -= 1;
      delay(500);
    }
  }
  
  if(jawTriggerState == 0)
  {
    digitalWrite(RED_LED, 1);
    digitalWrite(GREEN_LED, 0);
    digitalWrite(BLUE_LED, 0);
    // Start playing a file, then we can do stuff while waiting for it to finish
    if (! musicPlayer.startPlayingFile(audioFiles[index].c_str())) {
      Serial.print("Could not open file: ");
      Serial.print(audioFiles[index].c_str());
      while (1);
    }
    Serial.print(F("Started playing file: "));
    Serial.print(audioFiles[index].c_str());
  
    while (musicPlayer.playingMusic) {
      // file is now playing in the 'background' so now's a good time
      // to do something else like handling LEDs or buttons :)
      Serial.print(".");
      delay(1000);
    }
    Serial.println("Done playing sound");
    delay(1000);
  }
  else
  {
    digitalWrite(RED_LED, 0);
    digitalWrite(GREEN_LED, 0);
    digitalWrite(BLUE_LED, 1);
  }
}




/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
