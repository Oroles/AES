#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Keyboard.h"

static const byte KEY_SIZE = 16;
static const byte HASH_SIZE = 32;
static const byte PASSWORD_SIZE = 100;
static const byte MESSAGE_SIZE = 200;
static const int START_KEY = 900;
static const int START_HASH = 950;

static char key[KEY_SIZE];

void readKey() {
  for ( int i = 0; i < KEY_SIZE; ++i ) {
    //key[i] = ( i % 8 ) + '0';
    key[i] = EEPROM.read(i + START_KEY);
  }
}

void readHash(char* hash) {
  for( int i = 0; i < HASH_SIZE; ++i ) {
    hash[i] = EEPROM.read(i + START_HASH);
    //hash[i] = ( i % 8 ) + '0';
  }
}

void writeHash(char* hash) {
  for ( int i = 0; i < HASH_SIZE; ++i ) {
    EEPROM.write(i + START_HASH, hash[i]);
  }
}

char passwordBuffer[PASSWORD_SIZE];
void storeInBuffer( char* message )
{
  memset(passwordBuffer, '\0', PASSWORD_SIZE);
  memcpy(passwordBuffer, message, strlen(message) + 1);
}

void sendAsKeyboard(char* message)
{
  Keyboard.begin();
  Keyboard.print(message);
  Keyboard.end();
}

static int lastButtonStatus = LOW;
void sendPasswordAsKeyboard(int buttonStatus)
{
  if (buttonStatus != lastButtonStatus) {
     if (buttonStatus == HIGH) {
       sendAsKeyboard(passwordBuffer);
       memcpy(passwordBuffer, '\0', PASSWORD_SIZE);
     }
  }
  lastButtonStatus = buttonStatus;
}

void sendToSerial(char *message)
{
  int i = 0;
  while(message[i] != '\0') {
    Serial.print(message[i++]);
  }
}

void sendToBluetooth(SoftwareSerial* serial, char* message)
{
  int i = 0;
  while( message[i] != '\0' ) {
    serial->print(message[i++]);
  }
}

void sendToBluetoothHEX(SoftwareSerial* serial, char *message, int l)
{
  int i =0;
  for(i=0; i<l; ++i) {
    if ((unsigned char) message[i] < 0x10 ) {
      serial->print(0x00, HEX);
    }
    serial->print((unsigned char)message[i], HEX);
  }
}

void serialProcessRequest(SoftwareSerial* bluetoothSerial, char* inputString)
{
  char password[PASSWORD_SIZE];
  char encryptedPassword[PASSWORD_SIZE]; //ecrypted password will be on 16 bytes
  char message[MESSAGE_SIZE]; //message that will be send to the bluetooth

  unsigned long expansionKey[44];
  memset(expansionKey, 0, 44 * 4);
  
  memset(password, 0, PASSWORD_SIZE);
  memset(encryptedPassword, 0, PASSWORD_SIZE);
  memset(message, 0, MESSAGE_SIZE);
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      // add new entry

      // process key
      keyExpansion(key, expansionKey);
      getLastMessage(inputString, password);

      if ( strlen(password) % KEY_SIZE == 0 ) {
        //corect size we can process it

        //encrypte message
        int steps = strlen(password) / KEY_SIZE;
        int contor = 0;
        while ( contor < steps ) {
          cipher(&password[contor * KEY_SIZE], expansionKey, &encryptedPassword[contor * KEY_SIZE]);
          ++contor;
        }

        //generate request
        generateBluetoothAddMessage(inputString, message);
      
        //send message
        sendToBluetooth(bluetoothSerial, message);
        sendToBluetoothHEX(bluetoothSerial, encryptedPassword, strlen(password));
        bluetoothSerial->print("\n");

      } else {
        Serial.print(F("1:Fail\n")); //remove hardcoded part
      }

      break;
    case 2:
      // retrive entry
      sendToBluetooth(bluetoothSerial, inputString);
      break;
    case 3:
      // delete entry - no need to process, send directly to phone
      sendToBluetooth(bluetoothSerial, inputString);
      break;
    case 4:
      // obtain websites - no need to process, send directly to phone
      sendToBluetooth(bluetoothSerial, inputString);
      break;
    case 5:
      // close bluetooth the connection - no need to process, send directly to phone
      sendToBluetooth(bluetoothSerial, inputString);
    default:
      break;   
  }
}

void bluetoothProcessReply(SoftwareSerial* bluetoothSerial, char *inputString)
{
  char encryptedPassword[2 * PASSWORD_SIZE];
  char shortEncryptedPassword[PASSWORD_SIZE];
  char password[PASSWORD_SIZE];
  char message[MESSAGE_SIZE];
  
  memset(encryptedPassword, 0, 2 * PASSWORD_SIZE);
  memset(shortEncryptedPassword, 0, PASSWORD_SIZE);
  memset(password, 0, PASSWORD_SIZE);
  memset(message, 0, MESSAGE_SIZE);

  /*unsigned long expansionKey[44];
  memset(expansionKey, 0, 44 * 4);*/
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      {// add new entry
        sendToSerial(inputString);
      }
      break;
    case 2:
      // retrive entry

      //keyExpansion(key, expansionKey);
      getLastMessage(inputString, encryptedPassword);
      //encryptedPassword[ KEY_SIZE * (strlen(encryptedPassword) / KEY_SIZE)] = '\0'; //this comes from the fact,that when I send the password on phone it ends with \r\n
      generateShortPassword(encryptedPassword, shortEncryptedPassword);

      if ( strlen(encryptedPassword) % ( KEY_SIZE * 2 ) == 0 ) {
        unsigned long expansionKey[44];
        memset(expansionKey, 0, 44 * 4);
        keyExpansion(key, expansionKey);
        
        int steps = strlen(encryptedPassword) / (KEY_SIZE * 2);
        int contor = 0;
        while( contor < steps ) {
          invCipher(&shortEncryptedPassword[contor * KEY_SIZE], expansionKey, &password[contor * KEY_SIZE]);
          ++contor;
        }

        generateSerialRetriveMessage(password, message);
        storeInBuffer(message);
      }
      break;
    case 3:
      { // delete entry
        sendToSerial(inputString);
      }
      break;
    case 4:
      { // obtain websites
        sendToSerial(inputString);
      }
      break;
    case 5:
      { // get from message and store the hash value and close connection
        char hash[HASH_SIZE]; //input = "5:hash_value\n
        memset(hash, '\0', HASH_SIZE);
        getLastMessage(inputString, hash);
        writeHash(hash);
        generateSerialClose(message); //message = "5\n"
        sendToSerial(message);
      }
      break;
    case 6:
      { //read hash from EERPOM and send back to bluetooth
        char hash[HASH_SIZE]; //input = 6:\n
        memset(hash, '\0', HASH_SIZE);
        readHash(hash);
        generateBluetoothRetrieveHash(hash, HASH_SIZE, message); //message = 6:hash_value\n
        sendToBluetooth(bluetoothSerial, message);
      }
      break;
    default:
      // error, so ignore data
      break;   
  }
}
