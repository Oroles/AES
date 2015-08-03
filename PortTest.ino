#include <SoftwareSerial.h>

#include "serialcommunication.h"
#include "aes.h"
#include "utils.h"

// buffer for serial communication
char inputSerial[100];
unsigned int sizeInputSerial = 0;
boolean serialComplete = false;

// buffer for bluetooth communication
char inputBluetooth[100];
unsigned int sizeInputBluetooth = 0;
boolean bluetoothComplete = false;

// interface to bluetooth
SoftwareSerial* bluetoothSerial = new SoftwareSerial(10, 11);

// process the message received on the serial
void receiveSerialMessage()
{
  if (serialComplete) {
    serialProcessRequest(bluetoothSerial, inputSerial);
    //bluetoothSerial->write(inputSerial);

    // clear data
    memset(&inputSerial[0], 0, 100);
    sizeInputSerial = 0;
    serialComplete = false;
  }
}

// process the message received on the bluetooth
void receiveBluetoothMessage()
{
  if (bluetoothComplete) {
    bluetoothProcessReply(inputBluetooth);
    //Serial.write(inputBluetooth);
    
    // clear data
    memset(&inputBluetooth[0], 0, 100);
    sizeInputBluetooth = 0;
    bluetoothComplete = false;
  }
}

// read data on the serial
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputSerial[sizeInputSerial++] = inChar;
    
    // check not to overflow buffer
    if ( sizeInputSerial >= 100 )
    {
      //read to much data so ignore command, therefore
      sizeInputSerial = 0;
      break;
    }
    
    if (inChar == '\n') {
      serialComplete = true;
      break;
    }
  }
}

// read data on the bluetooth
void bluetoothEvent() {
  while (bluetoothSerial->available()) {
    // get the new byte;
    char inChar = (char)bluetoothSerial->read();
  
    // check not to overflow buffer
    if( sizeInputBluetooth >= 100 )
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
}

void loop() {
   receiveSerialMessage();
   receiveBluetoothMessage();
   serialEvent();
   bluetoothEvent();
}
