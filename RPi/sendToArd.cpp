//////////////////////////////////////////////
//	Eine aufgeräumte Verson des Originales	//
//	IN ARBEIT								//
//////////////////////////////////////////////

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;


//Prototypen
int getIndexFromInput(int);
string getValueFromInput(int);
void prepareCode();
void prepareCodeEfficient();
void send();
void printInfo();

//TODO: Als enum formulieren
const int iID = 1;
const int iMODE = 2;
const int iVOne = 3;
const int iVTwo = 4;
const int iVThree = 5;


const int PIN = 11;		//0 für Domiko

//Buffer für den zu sendenden Code
char buffer[32];

//Zu sendender Code
unsigned long code = 123;

//Argumente global verfügbar machen... nicht so schön, ich weiß... lass mich...
char** arg_v;
int arg_c;

struct MODE{
	string mName;
	string mToAppend;
	MODE(string name, string toAppend){
		mName = name;
		mToAppend = toAppend;
	}
};
struct ID{
	string mName;
	string mToAppend;
	ID(string name, string toAppend){
		mName = name;
		mToAppend = toAppend;
	}
};


//Neue Funktionen hier hinzufügen
ID ids[] = {
	ID("ALL", "0"),
	ID("WHITECON", "1"),
	ID("RGBCON", "2")
};

MODE modes[] = {
	MODE("OFF", "0"),
	MODE("FIXED", "1"),
	MODE("AUTO", "2")
};



int main(int argc, char **argv){
	arg_v = argv;
	arg_c = argc;

	prepareCodeEfficient();
	send();
	return 0;
}



void send(){
	if (wiringPiSetup () == -1) cout << "Error on initiate wiringPi";
	else{
	printf("Code sended: %ld \n", code);
	RCSwitch mySwitch = RCSwitch(); //Create a RCSwitch Object
	mySwitch.enableTransmit(PIN); //Enable transmission on PIN

	mySwitch.send(code, 32); // Send code in decimal
	}
}



//ID oder MODE id zurück liefern
int getIndexFromInput(int argument){
	bool found = false;
	if (arg_c > argument){
		//Prüfe ob übergebenes Argument einem Mode entspricht
		for (int i = 0; i < sizeof(modes)/sizeof(modes[0]); i++){
			string test = arg_v[2];
			string test2 = modes[i].mName;
			if (argument == iID){
				if (!strcmp(ids[i].mName.c_str(), arg_v[argument])){
					return i;
				}
			}
			else if (argument == iMODE){
				if (!strcmp(modes[i].mName.c_str(), arg_v[argument])){
					return i;
				}
			}
		}
	}
	return -1;
}


//Werte einlesen
string getValueFromInput(int argument){
	if (arg_c > argument){
		return arg_v[argument];
	}
}

//Hier wird der zu sendende Code zusammengesetzt
//ACHTUNG! Arduino kann maximal 32 bit empfangen! Drei Variablen sprengen die Kapazität.
//1+1+3+3+3 = 11 Stellen -> Zahlen bis 99.255.255.255; 4.294.967.295 maximal Möglich

//LÖSUNGSVORSCHLAG: Variablen in bytes senden statt Dezimal:
//1. Byte -> ID und Modus	-> i i i i m m m m		id[max] = mode[max] = 2^4 = 16
//2. Byte -> Variable 1 	-> 1 1 1 1 1 1 1 1		var[max] = 2^8 = 256
//3. Byte -> Variable 2		-> 2 2 2 2 2 2 2 2		var[max] = 2^8 = 256
//4. Byte -> Variable 3		-> 3 3 3 3 3 3 3 3 		var[max] = 2^8 = 256

// i i i i m m m m | 1 1 1 1 1 1 1 1 | 2 2 2 2 2 2 2 2 | 3 3 3 3 3 3 3 3

//Veraltet und Uneffizient
//
void prepareCode(){
	strcpy(buffer, ids[getIndexFromInput(iID)].mToAppend.c_str());
	strcat(buffer, modes[getIndexFromInput(iMODE)].mToAppend.c_str());
	strcat(buffer, getValueFromInput(iVOne).c_str());
	strcat(buffer, getValueFromInput(iVTwo).c_str());
	strcat(buffer, getValueFromInput(iVThree).c_str());
	cout << "buffer: " << buffer << endl;
	code = strtol(buffer, NULL, 10);
	long dcode = strtol(buffer, NULL, 10);
	cout << "Code: " << code << endl;
	cout << "Code2: " << dcode << endl;
}


//Getestet und als mega geil empfunden :D
void prepareCodeEfficient(){
	byte idAndMode = strtol(ids[getIndexFromInput(iID)].mToAppend.c_str(), NULL, 10) << 4;
	idAndMode += strtol(modes[getIndexFromInput(iMODE)].mToAppend.c_str(), NULL, 10);

	byte varOne = strtol(getValueFromInput(iVOne).c_str(), NULL, 10);
	byte varTwo = strtol(getValueFromInput(iVTwo).c_str(), NULL, 10);
	byte varThree = strtol(getValueFromInput(iVThree).c_str(), NULL, 10);

	code = idAndMode << 24;
	code += varOne << 16;
	code += varTwo << 8;
	code += varThree;
	
}



void printInfo(){
	printf("Arduino Command\n");
	printf("----------------\n");
	printf("ERROR: You didn't provide enough arguments! ");
	char buffer[20];
	printf("%d ", arg_c);
	printf("Are not enough\n");
	printf("./sendToArd [id] [mode] [value1] [value2] [value3]\n\n");
	printf("id: ");
	for (int i = 0; i < (sizeof(ids) / sizeof(ids[0])); i++){
		cout << ids[i].mName.c_str() << " ";
	}
	printf("\nmode: ");
	for (int i = 0; i < (sizeof(modes) / sizeof(modes[0])); i++){
		cout << modes[i].mName.c_str() << " ";
	}

	printf("\nvalue1 value2 value3 are variable");
}
