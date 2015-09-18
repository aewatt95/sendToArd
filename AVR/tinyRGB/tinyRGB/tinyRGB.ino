/*
 Author: Andre Eberhard
 Description: 	This program receive commands from an Raspberry PI
                Its used to control two light strips: cold white and warm white

You will need rcswitch library
http://code.google.com/p/rc-switch/
*/
  //------------------------------------------------------------------------------------------
  //Signal in Form: [ID] [MODE] [VALUE1] [VALUE2] [VALUE3]
  //For example      10    10      255      025        -  		
  //                 20    20      125      012       000  		//not implemented yet
  //First Bit is ID of the module, in this case 1 => 10000000
  //------------------------------------------------------------------------------------------

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();


//Define Pins where the strips are connected
int rPin = 0;
int gPin = 1;
int bPin = 2; // muss man noch anpassen - PWM implementieren

// Define Device Id
int device = 3;


//initialize Actual PWM output value [0, 255]
int rValue, gValue, bValue;
//initialize Last Value used for a soft change between values
int lastRValue, lastGValue, lastBValue; 

void setup() {
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2 on Atmega and Attiny
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT); 
  pinMode(bPin, OUTPUT);
  rValue = 0;
  gValue = 0;
  bValue = 0;
  lastRValue = 0;
  lastGValue = 0;
  lastBValue = 0;
}

void loop() {
  //If a message is sended with RF Code
  if (mySwitch.available()){ 

    // Copy the value 13digits long
    unsigned long message = mySwitch.getReceivedValue();
    
    unsigned int id = (int)(message/100000000000);
    message -= id*100000000000;
    
    unsigned int mode = (int)(message/1000000000);
    message -= mode*1000000000;
    
    unsigned int value1 = (int)(message/1000000);
    message -= value1*1000000;
    
    unsigned int value2 = (int)(message/1000);
    message -= value2*1000;
    
    unsigned int value3 = (int)(message);
    
    
    if (Id == device){ 
      switch(mode){
        //Mode: OFF ---------------------------------------------------------------------------
        case 0:{
          digitalWrite(rPin, LOW);
          digitalWrite(gPin, LOW);
          digitalWrite(bPin, LOW) ;
          lastRValue = 0;
          lastGValue = 0;
          lastBValue = 0;
        }
  	  
        //Mode: FIXED---------------------------------------------------------------------------
        case 1:{
          //Values are color Values
          rValue = value1; 
          gValue = value2;
          bValue = value3;
  		 
           //Soft change: Values reached in 1 second
  	   //TODO: Move to the main loop (No Code detecting while fading)
           for(int i = 0; i < 100; i++){
             analogWrite(rPin, lastRValue + ((rValue-lastRValue)/100.0f)*i);
             analogWrite(gPin, lastGValue + ((gValue-lastGValue)/100.0f)*i);
             analogWrite(bPin, lastBValue + ((bValue-lastBValue)/100.0f)*i);
             delay(10);
           }
           lastRValue = rValue;
           lastGValue = gValue;
           lastBValue = bValue;
        }
    	  
        //Mode: AUTO:---------------------------------------------------------------------------
        case 2:{
          //TODO
        }
      }    
    }
  mySwitch.resetAvailable();
  }   
}
