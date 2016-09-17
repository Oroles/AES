#include <SoftwareSerial.h>

#include "serialcommunication.h"
#include "utils.h"
#include "setkey.h"

// buffer for serial communication
char inputSerial[INPUT_SIZE];
boolean serialComplete = false;

// buffer for bluetooth communication
char inputBluetooth[INPUT_SIZE];
unsigned char sizeInputBluetooth = 0;
boolean bluetoothComplete = false;

const int BUTTON_PIN = 2;

// interface to bluetooth
SoftwareSerial* bluetoothSerial = new SoftwareSerial(10, 11);

// process the message received on the bluetooth
void receiveBluetoothMessage()
{
  if (bluetoothComplete) {
    bluetoothProcessReply(bluetoothSerial, inputBluetooth);
    
    // clear data
    memset(&inputBluetooth[0], 0, INPUT_SIZE);
    sizeInputBluetooth = 0;
    bluetoothComplete = false;
  }
}

// read data on the bluetooth
void bluetoothEvent() {
  while (bluetoothSerial->available()) {
    // get the new byte;
    char inChar = (char)bluetoothSerial->read();
  
    // check not to overflow buffer
    if( sizeInputBluetooth >= INPUT_SIZE )
    {
      //read to much data so ignore command, therefore
      sizeInputBluetooth = 0;
      break;
    }
    
    inputBluetooth[sizeInputBluetooth++] = inChar;
    if (inChar == '\n') {
      bluetoothComplete = true;
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  bluetoothSerial->begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  memset(inputSerial, INPUT_SIZE, '\0');
  memset(inputBluetooth, INPUT_SIZE, '\0'); 
}

enum ProgramMode { SET_KEY, PASSWORD_MANAGER };
ProgramMode mode = PASSWORD_MANAGER;

void serialEvent() {
  while (Serial.available()) {
    mode = SET_KEY;
    readInput(inputSerial);
  }
}

void loop() {
   serialEvent();
   if (mode == PASSWORD_MANAGER) {
    bluetoothEvent();
    receiveBluetoothMessage();
   } else {
    processKey(inputSerial, INPUT_SIZE);
   }
   
   sendDataFromBuffers(bluetoothSerial, digitalRead(BUTTON_PIN));
}
