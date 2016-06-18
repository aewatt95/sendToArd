/*
 Author: Andre Eberhard
 Description: 	This program receive commands from an Raspberry PI
                Its used to control RGB Led's.

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


//Define Pins where the strips are connected
/*int rPin = 0;
int gPin = 1;
int bPin = 4; // muss man noch anpassen - PWM implementieren
*/

int rPin = 11;
int gPin = 12;
int bPin = 13;


// Define Device Id
int deviceID = 2;
byte changeSpeed = 1;
byte pwmMax = 255;

//define Actual PWM output value [0, 255]
int rValue, gValue, bValue;
//define Last Value used for a soft change between values
int lastRValue, lastGValue, lastBValue;

byte localMode = 0;
byte allOnBrightness = 0;

void setup() {
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2 on Atmega and Attiny
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  digitalWrite(bPin, HIGH);
  delay(500);
  digitalWrite(bPin, LOW);
  rValue = 0;
  gValue = 0;
  bValue = 0;
  lastRValue = 0;
  lastGValue = 0;
  lastBValue = 0;
  
  Serial.begin(9600);
}

void loop() {
  //If a message is sended with RF Code
  if (mySwitch.available()) {

    // Get the received value
    unsigned long message = mySwitch.getReceivedValue();

    byte id = (message >> 28);
    byte mode = (message << 4) >> 28;
    byte valueOne = (message << 8) >> 24;
    byte valueTwo = (message << 16) >> 24;
    byte valueThree = (message << 24) >> 24;

    Serial.println(id);
    Serial.println(mode);
    Serial.println(valueOne);
    Serial.println(valueTwo);
    Serial.println(valueThree);
    
    if (id == deviceID) {
      switch (mode) {
        //Mode: OFF ---------------------------------------------------------------------------
        case 0: {
            localMode = 0;
            break;
          }

        //Mode: FIXED---------------------------------------------------------------------------
        case 1: {
            //Values are color Values
            rValue = valueOne;
            gValue = valueTwo;
            bValue = valueThree;
            localMode = 1;
            break;
          }

        //Mode: AUTO:---------------------------------------------------------------------------
        case 2: {
            changeSpeed = valueOne;
            localMode = 2;
            break;
          }

        //Mode: ALLON:--------------------------------------------------------------------------
        case 3: {
            rValue = valueOne;
            gValue = valueOne;
            bValue = valueOne;
            localMode = 1;
            break;
          }
      }
    }

    mySwitch.resetAvailable();

    switch (localMode) {
      case 0:
        {
          rValue = 0;
          gValue = 0;
          bValue = 0;
          lastRValue = 0;
          lastGValue = 0;
          lastBValue = 0;
          break;
        }
      case 1:
        {
          //Soft change: Values reached in 1 second
          //TODO: Move to the main loop (No Code detecting while fading)
          byte rTmp = rValue;
          byte gTmp = gValue;
          byte bTmp = bValue;
          
          for (int i = 0; i < 100; i++) {
            rValue = lastRValue + ((rTmp - lastRValue) / 100.0f)*i;
            gValue =  lastGValue + ((gTmp - lastGValue) / 100.0f)*i;
            bValue =  lastBValue + ((bTmp - lastBValue) / 100.0f)*i;
            draw();
            delay(10);
          }
          lastRValue = rValue;
          lastGValue = gValue;
          lastBValue = bValue;
          rValue = rTmp;
          gValue = gTmp;
          bValue = bTmp;
          break;
        }
      case 2:
        {
          hueRun();
          break;

        }
        
    }
  }
  draw();
}

  void draw() {
    if (lastRValue != rValue || lastGValue != gValue || lastBValue != bValue) {
      lastRValue = rValue;
      lastGValue = gValue;
      lastBValue = bValue;

      /*
       //Using Pin 4 on ATTiny85 for pwm poutput
       // Timer 1
       TCCR1 = _BV (CS10);           // no prescaler
       GTCCR = _BV (COM1B1) | _BV (PWM1B);  //  clear OC1B on compare
       OCR1B = bValue;
       OCR1C = 255;
       */
      analogWrite(bPin, bValue);
      analogWrite(rPin, rValue);
      analogWrite(gPin, gValue);
    }  // end of setup
  }


  //Convert a value of the HSV Color model into rgbValues... the simpel way... without trigonometrical function... for reason
  void calcHue(float degree) {
    if (degree >= 360.0f) degree = degree - (((int)(degree / 360)) * 360);
    if (degree < 60) {
      rValue = pwmMax;
      gValue = (pwmMax / 60) * degree;
      bValue = 0;
    }
    else if (degree < 120) {
      rValue = pwmMax - (pwmMax / 60) * (degree - 60);
      gValue = pwmMax;
      bValue = 0;
    }
    else if (degree < 180) {
      rValue = 0;
      gValue = pwmMax;
      bValue = (pwmMax / 60) * (degree - 120);
    }
    else if (degree < 240) {
      rValue = 0;
      gValue =  pwmMax - (pwmMax / 60) * (degree - 180);
      bValue  = pwmMax;
    }
    else if (degree < 300) {
      rValue = (pwmMax / 60) * (degree - 240);
      gValue =  0;
      bValue = pwmMax;
    }
    else {
      rValue = pwmMax;
      gValue =  0;
      bValue = pwmMax - (pwmMax / 60) * (degree - 300);
    }
  }


  unsigned long hueTime = 0;
  float hue = 0.0f;

  //Atumatic color change
  void hueRun() {
    if (millis() - hueTime > 10)
    {
      hueTime = millis();
      if (hue < 360)  hue = hue + (0.2f * (changeSpeed * 0.01f)) ;
      else hue = 0.0f;
      calcHue(hue);
    }
  }

