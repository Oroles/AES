#include "buttonoperation.h"
#include "Keyboard.h"

const byte PASSWORD_SIZE = 100;

bool enableOtherBluetoothOperations = false;
int lastButtonStatus = LOW;
MessageReceiver messageReceiver = None;
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

void storeInDataBuffer( char * message )
{
  memset(dataBuffer, '\0', PASSWORD_SIZE);
  memcpy(dataBuffer, message, strlen(message) + 1);
}

void sendDataFromBuffers(SoftwareSerial* bluetoothSerial, int buttonStatus)
{
  if (buttonStatus != lastButtonStatus) {
     if (buttonStatus == HIGH) {
       if (messageReceiver == Phone) {
        sendToBluetooth(bluetoothSerial, dataBuffer);
       }
       else {
        if (messageReceiver == Pc) {
         sendAsKeyboard(dataBuffer); 
        }
       }
       messageReceiver = None;
       memcpy(dataBuffer, '\0', PASSWORD_SIZE);
     }
  }
  lastButtonStatus = buttonStatus;
}

void setMessageReceiver(MessageReceiver receiver)
{
  messageReceiver = receiver;
}

void setEnableBluetoothOperations(boolean enable)
{
  enableOtherBluetoothOperations = enable;
}

boolean getEnableBluetoothOperations()
{
  return enableOtherBluetoothOperations;
}

