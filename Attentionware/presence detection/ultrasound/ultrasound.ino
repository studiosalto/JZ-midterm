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

void setup()
{
  Serial.begin(115200);
  Serial.println(" what" );
  Serial.println(" hoi " );
  //delay(1);
  Serial.println(" eh>");
  delay(1);
  pinMode(ultraSoundSignal,OUTPUT);
  
  maxDistance = ping();
  sensorTriggered = false;
}

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

void loop()
{

  // 00 - doing reading - skip when playing sound

  
  int x = 0;
  x = ping();
  //if (x!=0)
  if(x < maxDistance - typOffsetValue){
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
    Serial.println("@@@#$%$^@%*@&*$^(^$%@ FCK");
    
  }
  Serial.println(x);
  
  // 00 - doing reading - skip when playing sound


  delay(50); //delay 1/4 seconds.
  
}
