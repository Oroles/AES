#include "buttonoperation.h"
#include "Keyboard.h"

const byte PASSWORD_SIZE = 100;

bool enableOtherBluetoothOperations = false;
int lastButtonStatus = LOW;
LastOperation lastOperation = None;
char dataBuffer[PASSWORD_SIZE];

void sendAsKeyboard(const char* message)
{
  if (strlen(message) != 0) {
    Keyboard.begin();
    Keyboard.print(message);
    Keyboard.end();
  }
}

void sendToBluetooth(SoftwareSerial* serial, const char* message)
{
  int i = 0;
  while( message[i] != '\0' ) {
    serial->print(message[i++]);
  }
}

void sendToSerial(const char *message)
{
  int i = 0;
  while(message[i] != '\0') {
    Serial.print(message[i++]);
  }
}

void storeInDataBuffer( char * message )
{
  memset(dataBuffer, '\0', PASSWORD_SIZE);
  memcpy(dataBuffer, message, strlen(message) + 1);
}

void sendDataFromBuffers(SoftwareSerial* bluetoothSerial, int buttonStatus)
{
  if (buttonStatus != lastButtonStatus) {
     if (buttonStatus == HIGH) {
       if (lastOperation == Pc) {
        sendToBluetooth(bluetoothSerial, dataBuffer);
       }
       else {
        if (lastOperation == Phone) {
         sendAsKeyboard(dataBuffer); 
        }
        else {
          if (lastOperation == Button) {
            sendToBluetooth(bluetoothSerial, dataBuffer);
            enableOtherBluetoothOperations = true;
          }
        }
       }
       lastOperation = None;
       memcpy(dataBuffer, '\0', PASSWORD_SIZE);
     }
  }
  lastButtonStatus = buttonStatus;
}

void setLastOperation(LastOperation operation)
{
  lastOperation = operation;
}

void setEnableBluetoothOperations(boolean enable)
{
  enableOtherBluetoothOperations = enable;
}

boolean getEnableBluetoothOperations()
{
  return enableOtherBluetoothOperations;
}

