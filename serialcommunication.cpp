#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include <Arduino.h>

static const int KEY_SIZE = 16; //needs extra one for '\0'

static unsigned long expansionKey[44];
static char key[KEY_SIZE];

void sendSerialReply( char *message )
{
  int i = 0;
  while( message[i] != '\0' ) {
    Serial.write(message[i++]);
  }
}

void sendBluetoothRequest(SoftwareSerial* serial, char* message)
{
  int i = 0;
  while( message[i] != '\0' ) {
    serial->print(message[i++]);
  }
}

void sendBluetoothRequest(SoftwareSerial* serial, char *message, int l)
{
  int i =0;
  for(i=0; i<l; ++i) {
    if ((unsigned char) message[i] < 0x10 ) {
      serial->print(0x00, HEX);
    }
    serial->print((unsigned char)message[i], HEX);
  }
}

void serialProcessRequest(SoftwareSerial* serial, char* inputString)
{
  char password[KEY_SIZE]; //no longer than 16 characters
  char encryptedPassword[KEY_SIZE]; //ecrypted password will be on 16 bytes
  char message[100]; //message that will be send to the bluetooth
  char retrieveEntryMessage[100];
  
  memset(password,0,KEY_SIZE);
  memset(encryptedPassword,0,KEY_SIZE);
  memset(message,0,100);
  memset(retrieveEntryMessage,0,100);
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      // add new entry
      getPassword(inputString, password);
      getKey(inputString, key);
      keyExpansion(key, expansionKey);
      cipher(password, expansionKey, &encryptedPassword);

      generateBluetoothAddMessage(inputString, encryptedPassword, message);
      
      //send message
      sendBluetoothRequest(serial, message);
      sendBluetoothRequest(serial, encryptedPassword, 16); //use this for password
      serial->println(""); 
      break;
    case 2:
      // retrive entry
      memset(key,0,16);
      getKey(inputString,key);
      generateBluetoothRetriveMessage(inputString,retrieveEntryMessage);
      sendBluetoothRequest(serial, retrieveEntryMessage);
      break;
    case 3:
      // delete entry - no need to process, send directly to phone
      sendBluetoothRequest(serial, inputString);
      break;
    case 4:
      // obtain websites - no need to process, send directly to phone
      sendBluetoothRequest(serial, inputString);
      break;
    default:
      memset(key, 0, KEY_SIZE);
      // error, so ignore data
      break;   
  }
}

void bluetoothProcessReply(char *inputString)
{
  char encryptedPassword[2 * KEY_SIZE];
  char shortEncryptedPassword[KEY_SIZE];
  char password[KEY_SIZE];
  char message[100];
  
  memset(encryptedPassword,0,2 * KEY_SIZE);
  memset(shortEncryptedPassword, 0, KEY_SIZE);
  memset(password,0,KEY_SIZE);
  memset(message,0,100);
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      // add new entry - ne need to process, send directly to serial
      // the message should be Ok or Fail
      sendSerialReply(inputString);
      break;
    case 2:
      // retrive entry
      getEncryptedPassword(inputString, encryptedPassword);
      generateShortPassword(encryptedPassword, shortEncryptedPassword);
      keyExpansion(key, expansionKey);
      invCipher(shortEncryptedPassword, expansionKey, &password);
      generateSerialRetriveMessage(password, message);
      sendSerialReply(message);
      break;
    case 3:
      // delete entry - no need to process, send directly to serial
      sendSerialReply(inputString);
      break;
    case 4:
      // obtain websites - no need to process, send directly to serial
      sendSerialReply(inputString);
      break;
    default:
      // error, so ignore data
      break;   
  }
}
