/*
 Author: Andre Eberhard
 Description: 	This program receive commands from an Raspberry PI
                Its used to control two light strips: cold white and warm white

You will need rcswitch library
http://code.google.com/p/rc-switch/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
//Define Pins where the strips are connected
int wPin = 0;
int cPin = 1;

//Actual PWM output value [0, 255]
int warmValue, coldValue;
//Last Value used for a soft change between values
int lastWValue, lastCValue; 


void setup() {
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2 on Atmega and Attiny
  pinMode(wPin, OUTPUT);
  pinMode(cPin, OUTPUT); 
  warmValue = 0;
  coldValue = 0;
  lastWValue = 0;
  lastCValue = 0;
}

void loop() {

  if (mySwitch.available()) { //If a message is sended with RF Code
   unsigned long value = mySwitch.getReceivedValue(); // Copy the value 
   
   
  //Signal in Form: [ID] [MODE] [VALUE1] [VALUE2] [VALUE3]
  //For example      1    1      255      025        -  		
  //                 2    2      125      012       000  		//not implemented yet
  //First Bit is ID of the module, in this case 1 => 10000000
   
   if (value >= 10000000)
   { 
   
     //using combinations of division and modulo to read different parts of recieved code 
     int mode = (value/1000000)%10;
     switch(mode){
	 
       //Mode: OFF
      case 0:{
        digitalWrite(wPin, LOW);
        digitalWrite(cPin, LOW); 
        lastWValue = 0;
        lastCValue = 0;
      }
	  
      //Mode: FIXED
      case 1:{
         warmValue = (value/1000)%1000; 
         coldValue = (value%1000);
		 
         //Soft change: Values reached in 1 second
	 //TODO: Move to the main loop (No Code detecting while fading)
         for(int i = 0; i < 100; i++){
           analogWrite(wPin, lastWValue + ((warmValue-lastWValue)/100.0f)*i);
           analogWrite(cPin, lastCValue + ((coldValue-lastCValue)/100.0f)*i);
           delay(10);
         }
         lastWValue = warmValue;
         lastCValue = coldValue;
      }
	  
      //Mode: AUTO:
      case 2:{
      } 
       //TODO 
     } 
 }
 mySwitch.resetAvailable();
}
   
}
