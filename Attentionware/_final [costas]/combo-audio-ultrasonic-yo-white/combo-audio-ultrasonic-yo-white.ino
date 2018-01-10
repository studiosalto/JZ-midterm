// AUDIO SETUP CODE STARTS HERE VVVVVV


#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "runningaverage.h"
#include "painlessMesh.h"

#define   MESH_PREFIX     "unit_zero"
#define   MESH_PASSWORD   "touchmyhardware"
#define   MESH_PORT       5555
#define LDR_READ_EVERY_MS 200 //sonar measurement every 200ms for calculating average
#define MAXIMUM_MP3_TIME 9000

/* ___________________WiFi Mesh Setup____________________*/
void sendMessage() ; // Prototype so PlatformIO doesn't complain
painlessMesh  mesh;
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

/* ___________________LDR Sonar Average____________________*/
RunningAverage ravg( (1000 / LDR_READ_EVERY_MS) * 5 );

/* ___________________MP3 Code____________________*/
SoftwareSerial mySoftwareSerial(D2, D3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

unsigned long echo = 0;
int ultraSoundSignal = 5; // Ultrasound signal pin (5 on an Arduino translates to D1 on WEMOS. @see https://github.com/esp8266/Arduino/issues/1243)
unsigned long ultrasoundValue = 0;

int randNumber=0;
float maxDetectionRange = 4; // 4 metres
float timeForSoundToTravelBackAndForth = 2 * maxDetectionRange / 340; // 340 = speed of sound. this value is in seconds
float maxDistance;

bool iDetectedSomebody=false;
bool isSomeoneInTheRoom = false;
bool sensorTriggered;
int minReqTriggers = 3;
float typOffsetValue = 5;
int readingsCount = 0;
int reading; //LDR sonar reading
int isTalking = false;
int myLastTriggeredState = 0;
int myState = 0;  //0: Noone is in the room (noone detected nobody, no sound) 
                  //1: Someone is in the room (detected by me or someone, avg<100) 
                  //2: Someone is close to me (detected by me, avg<60, [dont care what others do]) 
                  //3: Someone is "touching" me (detected by me, avg<30, [dont care what others do]) 
                  //4: I am jealous           (someone else is in State 3)

                  //ORIGINALLY:
                  //0: Noone is in the room (make sounds to attract someone, big delay=5min [could be disturbing]) 
                  //1: Someone is in the room (detected by me or others, avg<100) 
                  //2: Someone is "touching" me (detected by me, avg<30, [dont care what others do]) 
                  //3: I am jealous           (someone else is in State 3)

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

  echo = pulseIn(ultraSoundSignal, HIGH, (long)(timeForSoundToTravelBackAndForth * 1000000)); //Listen for echo
  ultrasoundValue = (echo / 58.138);

  return ultrasoundValue;
}

void init_mesh(){
  
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.scheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void init_ultrasound() {
  //delay(1);
  Serial.println(" eh>");
  delay(1);
  pinMode(ultraSoundSignal, OUTPUT);

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
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  //extras start
 // myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  myDFPlayer.enableDAC();
  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
 //extras end
  //myDFPlayer.play(1);  //Play the first mp3
}

void printDetail(uint8_t type, int value) {
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
      delay(200); //IMPORTANT! 200 millisecond (or 1 sec) are needed to start making average measurements correct again!
      isTalking=false;
      delay(50);
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
  
  if(!isTalking){    
    
    if(myState==1){
      //generate number 1 or 2 randomly (never 3).
      randNumber=random(1, 3); 
      myDFPlayer.pause(); 
      myDFPlayer.playFolder(randNumber, 1);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
      Serial.print("Someone is in the room! Random: ");
      Serial.print(randNumber);
      Serial.print("\n");   
      isTalking=true;       
    }
    else if(myState==2) 
    {
      myLastTriggeredState=myState;
      //generate number from 1 to 5 randomly (never 6).
      randNumber=random(1, 6);  
      myDFPlayer.pause();
      myDFPlayer.playFolder(randNumber, 2);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
      Serial.print("Someone is close to me! Random: ");
      Serial.print(randNumber);
      Serial.print("\n");    
      isTalking=true;    
    }
    else if(myState==3)
    {
      myLastTriggeredState=myState;
      //generate number from 1 to 5 randomly (never 6).
      randNumber=random(1, 6);  
      myDFPlayer.pause();
      myDFPlayer.playFolder(randNumber, 3);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
      Serial.print("Someone is touching me! Random: ");
      Serial.print(randNumber);
      Serial.print("\n");   
      isTalking=true;     
    }
    else if(myState==4)
    {
      myLastTriggeredState=myState;
      //generate number from 1 to 4 randomly (never 5).
      randNumber=random(1, 5);  
      myDFPlayer.pause();
      myDFPlayer.playFolder(randNumber, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
      Serial.print("I am so jealous now! Random: ");
      Serial.print(randNumber);
      Serial.print("\n");   
      isTalking=true;      
    }  
  }
}

void loop_mesh(){
  mesh.update();
}

void loop_ultrasound() {
  int reading = 0;

  reading = ping();
  ravg.addValue(reading);

  //if (x!=0)
  if (reading < maxDistance - typOffsetValue) {
    sensorTriggered = true;
  } else {
    sensorTriggered = false;
  }
  
  if (sensorTriggered) {
    readingsCount++;
  } else {
    readingsCount--;
  }
  
  if (readingsCount > minReqTriggers) readingsCount = minReqTriggers;
  if (readingsCount < 0) readingsCount = 0;
  if (readingsCount >= minReqTriggers) {    
    
    //myState 3: Someone is "touching" me (detected by me, avg<30, [dont care what others do]) 
    if(ravg.getAverage() < 65){
      myState=3;
      on_trigger(); //includes isTalking=true;
            
      //let them all be jealous
      //see sendMessage()
    
    }
 

    //myState 2: Someone is in the room (detected by me) 
    else if (ravg.getAverage() > 65 && ravg.getAverage() < 150 ) {
      myState=2;
      on_trigger();     
    }
    
    //myState 1: Noone is in the room but I mumble now
    else if (ravg.getAverage() >= 201 /*&& ravg.getAverage() < /* 180 */ /*132*/) { // NATURAL delay due to random nosie. the && are used for safety 
      
      myState=1;
      on_trigger(); //includes isTalking=true;

    }
    else
    {
      
      if(myState!=4) {
        isTalking=false; //CAREFUL
        myState=0;
      }
      
    }
    
  }
  Serial.print("raw: ");
  Serial.print( reading );
  Serial.print(",average: ");
  Serial.println( ravg.getAverage() );
  Serial.print(", readingsCount: ");
  Serial.println( readingsCount );
  Serial.print(", sensorTriggered: ");
  Serial.println( sensorTriggered );
  Serial.print(", isTalking: ");
  Serial.println( isTalking );
  Serial.print(", myState: ");
  Serial.print( myState );
  Serial.print(", iDetectedSomebody: ");
  Serial.println( iDetectedSomebody );
  Serial.print(", randNumber: ");
  Serial.println( randNumber );
  // 00 - doing reading - skip when playing sound
  delay(50); //delay 1/4 seconds.
} // loop_ultrasound()

void loop_player() {
//  static unsigned long timer = millis();
//  if (millis() - timer > 3000) {
//    timer = millis();
//    myDFPlayer.next();  //Play next mp3 every 3 second.
//  }


  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

/*____________________Message Sender__________________________*/
/*________Send messages to the other modules around___________*/
void sendMessage() {
    
    String msg = "null message"; //  msg += mesh.getNodeId();

    if(myState==3){
                                //Someone is touching me...
                                //...so let them become jealous
      msg = "Someone's touching!";
    }
    mesh.sendBroadcast( msg );
    taskSendMessage.setInterval( TASK_SECOND * 1);
//    iDetectedSomebody=false;
}

/*____________________Message Receiver__________________________*/
/*_____Check the messages received from other modules around_____*/
/*_Do something or send back another message to them if needed!_*/
void receivedCallback( uint32_t from, String &msg ) {
    if(!isTalking){      
      //check if I am busy with a user. If not, then bother me with your (shitty) messages...
      
                                  //      if(msg.equals("I REMOVEDDDD Detected Somebody!") && myState==0){
                                  //        Serial.println("_________________________Someone detected somebody around finally!");
                                  //        delay(random(3000,5000));
                                  //        myState=1;
                                  //        on_trigger();
                                  //        //delay(MAXIMUM_MP3_TIME); need to make sure this line runs only once.
                                  //      } else
                                  
      if(msg.equals("Someone's touching!") && (myState==0 || myState==1))
      {
        Serial.println("_____________________________________________Arggghhh! Touch me not him!");
        delay(random(300,1000));
        myState=4;
        on_trigger();
      }else{
        Serial.println("_____________________________________________Why so silent all around?");
        //if its any other message, e.g. a space (" "), then:
        myState = 0;
      }    
    }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200); //boom! different serial baud for WiFi and LDR.

  init_mesh();
  init_player();
  init_ultrasound();
  
}

void loop() {
  loop_mesh();
  loop_ultrasound();
  loop_player();

}



