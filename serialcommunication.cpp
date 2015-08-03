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

void sendSerialReply(char *message, int l)
{
  int i =0;
  for(i=0; i<l; ++i) {
    Serial.write(convertToHex(message[i]));
  }
}

void sendBluetoothRequest(SoftwareSerial* serial, char* message)
{
  int i = 0;
  while( message[i] != '\0' ) {
    serial->write(message[i++]);
  }
}

void sendBluetoothRequest(SoftwareSerial* serial, char *message, int l)
{
  int i =0;
  for(i=0; i<l; ++i) {
    serial->write(convertToHex(message[i]));
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
      
      sendBluetoothRequest(serial, message);
      sendBluetoothRequest(serial, encryptedPassword, 16);
      serial->println("");
      
      /*
      verific atat cu serial monitor cat si cu Qt
      1. verific in main ca merge algorithm-ul de cryptare/decryptare( aceeasi valoare )
      2. verific aici ca merge algorithm-ul de cryptare/decryptare
      3. verific ca pot sa trimite bytes
      4. fac perechi de 2 nibble si trimit la telefon( doar pentru parola )
      
      cand primesc trebuie sa primesc deja ca bytes si nu trebuie sa fac schimbari
      */
      
      
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
  char encryptedPassword[KEY_SIZE];
  char password[KEY_SIZE];
  char message[100];
  
  memset(encryptedPassword,0,KEY_SIZE);
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
      sendSerialReply(inputString);
      /*getEncryptedPassword(inputString, encryptedPassword);
      keyExpansion(key, expansionKey);
      invCipher(encryptedPassword, expansionKey, &password);
      generateSerialRetriveMessage(password, message);
      sendSerialReply(message);*/
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
