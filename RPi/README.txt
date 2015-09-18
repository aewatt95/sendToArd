This Folder contains the software and the librarys for the Raspberry Pi.


First install wiringPi by entering the directory and running sudo ./build

Compile the cpp files in the main directory with
gcc sendToArd.cpp RCSwitch.cpp -l wiringPi -o sendToArd

Run sudo ./sendToArd for instructions