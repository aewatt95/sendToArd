/*
 Author: Andre Eberhard
 Description:   This program receive commands from an Raspberry PI
                It can be used to control RGB LED's for example.
                Feel free to change whatever you need to.

You will need rcswitch library
http://code.google.com/p/rc-switch/
*/

//------------------------------------------------------------------------------------------
//Signal in Form:   [BYTE1]   [BYTE2]   [BYTE3]   [BYTE4]
//                  id+mode    var1      var2      var3
//maxValues         [16][16]   [256]    [256]     [256]     
//------------------------------------------------------------------------------------------

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

//Up to 16
int deviceID = 1;

//Up to 16
enum modes {ONE, TWO, THREE, FOUR};
void setup() {
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2 on Atmega and Attiny
}

void loop() {
  //If a message is sended with RF Code
  if (mySwitch.available()) {

    // Copy the value 13digits long
    unsigned long message = mySwitch.getReceivedValue();

    byte id = (message >> 28);
    byte mode = (message << 4) >> 28;
    byte valueOne = (message << 8) >> 24;
    byte valueTwo = (message << 16) >> 24;
    byte valueThree = (message << 24) >> 24;

    //Insert y
    if (id == deviceID) {
        switch(mode){
          case ONE:{
            break;
          }
          case TWO:{
            break;
          }
          case THREE:{
            break;
          }
          case FOUR:{
            break;
          }
        }
    }
  }
  mySwitch.resetAvailable();
}

