//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time betweew 1 and 5 seconds
// 2. prints anything it recieves to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <CapacitiveSensor.h>

#define   MESH_PREFIX     "unitzero"
#define   MESH_PASSWORD   "zerozero"
#define   MESH_PORT       5555

#define MIN_VALUE_MAP 0
#define MAX_VALUE_MAP 80

void sendMessage() ; // Prototype so PlatformIO doesn't complain
uint32_t moduleID = 0;
//unsigned int connectedNodes = 0;
int myState = 0;

int led = 0; 
int distanceSensor = A0;
bool userInRange = false;
bool userTouching = false;

CapacitiveSensor   myTouch = CapacitiveSensor(4,3);        // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire and or foil if desired

painlessMesh  mesh;
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void receivedCallback( uint32_t from, String &msg ) {
  
  //Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  //Serial.printf("%s\n", from, msg.c_str());
  
  if(msg.c_str()=="GetJealous!" /*&& (myState==0 || myState==1)*/){
    myState=3; 
    Serial.printf("I am jealous of %u!\n", from);
    //be jealous only if noone is around OR some is close
    //code to light up my leddddd acting jealously
    //random time delay so that not all modules start shoutign at the same time
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

void setup() {
  pinMode(distanceSensor, INPUT);
  pinMode(led, OUTPUT);
  myTouch.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);
//    Serial.begin(9600);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  moduleID = mesh.getNodeId();

  mesh.scheduler.addTask( taskSendMessage );
  taskSendMessage.enable() ;
}

void loop() {
  readAnalogSensor();
  mesh.update();
  delay(1); // <- fixes some issues with WiFi stability

//  unsigned int connCount = mesh.connectionCount();
//  if(connCount > connectedNodes) {
//    Serial.println("|<new_device_connected>|");
//  }
//  connectedNodes = connCount;
}


void sendMessage() {
  String msg = "";   

  if(myState==2){
  msg = "GetJealous!";
  //msg += mesh.getNodeId();
  }
  else
  {
    msg = "0";
  }
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(400); //400millisecs
  //taskSendMessage.setInterval(TASK_SECOND * 1); //checking every second
  //taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}



bool touchDetected(){
  
    long start = millis();
    long total1 =  myTouch.capacitiveSensor(30);
    Serial.println(total1);
    //the mroe the resistance the higher the value (1MΩ => ~700.0 value, 680KΩ => ~400.0 value)
    //1ΜΩ: 3000 value. With Acrylic on top: 1000 value. RESULT: WIth acrylic on top of an aluminum foil and 1MΩ resistor you get 1/3 of the initial value. Proximity still works good.
    //1MΩ: 2400 value. With Thin Latex 
    delay(10);
    return true;
//    if(totall>2400){
//      return true;
//    }else{
//      return false;
//    }
}

void readAnalogSensor() {
    if( millis() % 50 != 0 )
       return;
       
    int val = analogRead(distanceSensor);
    int mappedValue;
      mappedValue = map(val, 40, 925, 80, 0);
      if(mappedValue < MIN_VALUE_MAP) mappedValue = MIN_VALUE_MAP;
      if(mappedValue > MAX_VALUE_MAP) mappedValue = MAX_VALUE_MAP;
//     Serial.print("sns: ");
     Serial.println(val);
      
      if(mappedValue<72){
        userInRange = true;
        myState = 1; 
        //STATE 1
        //be ready to get jealous
        
//       if(touchDetected()){
//        
//        }
        
        if(mappedValue<50){
          userTouching=true;
          myState = 2;   
          
          //STATE 2
          //1. play 000001.mp3
          //2. prepare 000002.mp3
          //3. if(file==00015.mp3) prepare 000001.mp3 (loopagain through pickup lines)
          //
          //
                 
        }else{
          userTouching = false; 
          myState = 1;
          //STATE 1   
          //be ready to get jealous
        }
    
      }else{
        userInRange = false;
        myState=0;
        //STATE 0

        //be ready to get jealous
      }  
      //Serial.println(val);
 
}



