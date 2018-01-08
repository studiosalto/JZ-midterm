#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */

// Luis will modify the the CapacativeSensor library to accomodate Wemos. He suspects the code to 'expect' Arduino's 5 volts instead of Wemos 3.3V.
// We can keep testing with Arduino to 


CapacitiveSensor   cap = CapacitiveSensor(0, 4);        // 1M resistor between pins D2 & D3, pin 3 is sensor pin, add a wire and or foil if desired
// 3 = sender, 2 = receiver  // arduino = (3, 2);


void setup()                    
{
   cap.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Serial.begin(9600);
}

void loop()                    
{
    long start = millis();
    long total1 =  cap.capacitiveSensor(30);

    //Serial.print(millis() - start);        //  check on performance in milliseconds
    //Serial.print("\t");                    // tab character for debug windown spacing

    Serial.println(total1);                  // print sensor output 1

    delay(10);                             // arbitrary delay to limit data to serial port 
}
