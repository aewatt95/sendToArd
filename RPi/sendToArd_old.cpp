/*
 Author: Andre Eberhard
 Description: This program sends commands to a Arduino
			  Its used to controll multiple attinys for led controll in the first place
 
 Usage: ./sendToArd [id] [mode] [value1] [value2] [value3]
 Ex: ./sendToArd WHITECON FIXED 255 050
 
 You will need rcswitch-pi library / WiringPi
 https://github.com/r10r/rcswitch-pi
 
 To install WiringPI (it will clone the git in the current directory)
 wget http://smarturl.it/wiringpi
 chmod +x wiringpi
 ./wiringpi

 To compile it:
 gcc sendToArd.cpp -o sendToArd RCSwitch.cpp -l wiringPi

 Requirement : A RF Transmitter (on GPIO 7 / Pin 11 or redefined GPIO)  
 ---------------
 GPIO references
 http://elinux.org/images/2/2a/GPIOs.png
 ---------------
 WiringPI reference
 http://pi.gadgetoid.com/WiringPi-GPIO-Pinout.png?v=1
*/

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//Boah bist du hässlich... Ich will nix mehr mit dir zu tun haben. Es ist aus!
int main(int argc, char *argv[]) {

    int PIN = 11; //This is the pin of the transmitter
    char buffer[256]; //This variable serve as a buffer to copy the string

	
if (argc > 2) // Verify if there is enough arguments
	{
		if (!strcmp("WHITECON",argv[1]))
		{
			//ID for WHITECON = 1
			strcpy(buffer,"1");
		}
		else if(!strcmp("RGBCON"))
		{
			//ID for RGBCON = 2
			strcpy(buffer,"2");
		}

		printf("Controller %s selected \n",argv[1]);
		if(argc > 4){
				if (!strcmp("AUTO",argv[2])){
					//TODO or remove... not sure yet
					//ID for AUTO = 2
				}
				else if(!strcmp("FIXED",argv[2])){
					//ID for FIXED = 1
					strcat(buffer, "1");
					strcat(buffer, argv[3]);
					strcat(buffer, argv[4]);
					if(argc == 5){
						strcat(buffer, argv[5]);
					}
					else{
						strcat(buffer, "000");
					}
					printf("Sending %s ", argv[2]);
					printf("with warmValue = %s", argv[3]);
					printf(" and coldValue = %s \n", argv[4]);
				}
		}
		else if(argc == 3 && !strcmp("OFF",argv[2])){
			//ID for OFF = 0
			strcat(buffer, "0");
			strcat(buffer, "000000");
			printf("Sending %s signal\n", argv[2]);
		}
		else{
			printf("ERROR: Wrong mode or wrong values. Possible modes: \nAUTO FIXED OFF\n");
		}
	}
	
	else
	{
		printf("Arduino Command\n");
		printf("----------------\n");
		printf("ERROR: You didn't provide enough arguments! ");
		char buffer[20];
		printf("%d ", argc);
		printf("Are not enough\n");
		printf("./sendToArd [id] [mode] [value1] [value2] [value3]\n\n");
		printf("id: WHITECON, RGBCON\n");
		printf("mode: AUTO, FIXED, OFF\n");
		printf("value1 is warm white, R , speed value\n");
		printf("value2 is cold white or G value\n");
		printf("value3 is B value\n");
	}


    long code = strtol(buffer,NULL,10); // Convert our code (which is a string) to long
    printf("Code sended: %ld\n",code);

    if (wiringPiSetup () == -1) return 1;
        RCSwitch mySwitch = RCSwitch(); //Create a RCSwitch Object
        mySwitch.enableTransmit(PIN); //Enable transmission on PIN

        mySwitch.send(code,24); // Send code in decimal
        return 0;
}
