#include <SoftwareSerial.h>

#include "serialcommunication.h"
#include "aes.h"
#include "utils.h"

static const byte MESSAGE_SIZE = 100;

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

// process the message received on the serial
void receiveSerialMessage()
{
  if (serialComplete) {
    serialProcessRequest(bluetoothSerial, inputSerial);
    //bluetoothSerial->write(inputSerial);

    // clear data
    memset(&inputSerial[0], 0, MESSAGE_SIZE);
    sizeInputSerial = 0;
    serialComplete = false;
  }
}

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

// read data on the serial
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputSerial[sizeInputSerial++] = inChar;
    
    // check not to overflow buffer
    if ( sizeInputSerial >= MESSAGE_SIZE )
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
  readKey();
}

void loop() {
   receiveSerialMessage();
   receiveBluetoothMessage();
   serialEvent();
   bluetoothEvent();
   sendPasswordAsKeyboard(digitalRead(BUTTON_PIN));
}
