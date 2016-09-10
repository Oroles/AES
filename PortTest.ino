#include <SoftwareSerial.h>


#include "serialcommunication.h"
#include "aes.h"
#include "utils.h"

//static const byte MESSAGE_SIZE = 100;

// buffer for serial communication
char inputSerial[MESSAGE_SIZE];
unsigned char sizeInputSerial = 0;
boolean serialComplete = false;

// buffer for bluetooth communication
char inputBluetooth[MESSAGE_SIZE];
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
    //Serial.write(inputBluetooth);
    
    // clear data
    memset(&inputBluetooth[0], 0, MESSAGE_SIZE);
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
    if( sizeInputBluetooth >= MESSAGE_SIZE )
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
  bluetoothSerial->begin(9600);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
   receiveBluetoothMessage();
   bluetoothEvent();
   sendDataFromBuffers(bluetoothSerial, digitalRead(BUTTON_PIN));
}
