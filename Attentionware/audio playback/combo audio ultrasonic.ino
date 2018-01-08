// AUDIO SETUP CODE STARTS HERE VVVVVV


#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "runningaverage.h"


#define LDR_READ_EVERY_MS 200

RunningAverage ravg( (1000/LDR_READ_EVERY_MS) * 5 );

SoftwareSerial mySoftwareSerial(D2, D3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

unsigned long echo = 0;
int ultraSoundSignal = 5; // Ultrasound signal pin (5 on an Arduino translates to D1 on WEMOS. @see https://github.com/esp8266/Arduino/issues/1243)
unsigned long ultrasoundValue = 0;

float maxDetectionRange = 4; // 4 metres
float timeForSoundToTravelBackAndForth = 2 * maxDetectionRange / 340; // 340 = speed of sound. this value is in seconds
float maxDistance;

bool sensorTriggered;
int minReqTriggers = 3;
float typOffsetValue = 5;
int readingsCount = 0;

int reading;

unsigned long ping()
{
  pinMode(ultraSoundSignal, OUTPUT); // Switch signalpin to output
  digitalWrite(ultraSoundSignal, LOW); // Send low pulse
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(ultraSoundSignal, HIGH); // Send high pulse
  delayMicroseconds(5); // Wait for 5 microseconds
  digitalWrite(ultraSoundSignal, LOW); // Holdoff
  pinMode(ultraSoundSignal, INPUT); // Switch signalpin to input
  digitalWrite(ultraSoundSignal, HIGH); // Turn on pullup resistor
  // please note that pulseIn has a 1sec timeout, which may
  // not be desirable. Depending on your sensor specs, you
  // can likely bound the time like this -- marcmerlin
  // echo = pulseIn(ultraSoundSignal, HIGH, 38000)

  echo = pulseIn(ultraSoundSignal, HIGH, (long)(timeForSoundToTravelBackAndForth*1000000)); //Listen for echo
  ultrasoundValue = (echo / 58.138);
  
  return ultrasoundValue;
}

void init_ultrasound() {
  //delay(1);
  Serial.println(" eh>");
  delay(1);
  pinMode(ultraSoundSignal,OUTPUT);
  
  maxDistance = ping();
  sensorTriggered = false;
}

void init_player() {
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  //myDFPlayer.play(1);  //Play the first mp3
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void on_trigger() {
    Serial.println("I CAN SEE YOUR HAND!");
    myDFPlayer.play(1); // play track
}

void loop_ultrasound() {
  int reading = 0;
  reading = ping();
  ravg.addValue(reading);
  
  //if (x!=0)
  if(reading < maxDistance - typOffsetValue){
    sensorTriggered = true;
  }else{
    sensorTriggered = false;
  }
  if(sensorTriggered){
    readingsCount++;
    
  }else{
    readingsCount--;
  }
  if(readingsCount > minReqTriggers) readingsCount = minReqTriggers;
  if(readingsCount < 0) readingsCount = 0;
  if(readingsCount >= minReqTriggers){
    //Serial.println("@@@#$%$^@%*@&*$^(^$%@ FCK");
    //on_trigger(); 
  }
  //Serial.println(x);

  Serial.print( reading );
  Serial.print(",");
  Serial.println( ravg.getAverage() );
  
  // 00 - doing reading - skip when playing sound
  delay(50); //delay 1/4 seconds.
} // loop_ultrasound()

void loop_player() {
  static unsigned long timer = millis();
  
//    if (millis() - timer > 3000) {
//    timer = millis();
//    myDFPlayer.next();  //Play next mp3 every 3 second.
//  }
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

// /////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  init_ultrasound();
  init_player();
}

void loop() {
  loop_ultrasound();
  loop_player();
}



