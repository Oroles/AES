#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include "passwordgenerator.h"
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

void sendToSerial(const char *message)
{
  int i = 0;
  while(message[i] != '\0') {
    Serial.print(message[i++]);
  }
}

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


static char dataBuffer[PASSWORD_SIZE];
void storeInDataBuffer( char * message )
{
  memset(dataBuffer, '\0', PASSWORD_SIZE);
  memcpy(dataBuffer, message, strlen(message) + 1);
}

enum LastOperation{ Pc, Phone, None };

static LastOperation lastOperation = None;
static int lastButtonStatus = LOW;
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
       }
       lastOperation = None;
       memcpy(dataBuffer, '\0', PASSWORD_SIZE);
     }
  }
  lastButtonStatus = buttonStatus;
}

void serialProcessRequest(SoftwareSerial* bluetoothSerial, char* inputString)
{
  char password[PASSWORD_SIZE];
  char encryptedPassword[PASSWORD_SIZE];
  char message[MESSAGE_SIZE];
  
  memset(password, 0, PASSWORD_SIZE);
  memset(encryptedPassword, 0, PASSWORD_SIZE);
  memset(message, 0, MESSAGE_SIZE);
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      {
        // add new entry
        getLastMessage(inputString, password);
        if (encryptPassword(password, key, KEY_SIZE, encryptedPassword)) {
          generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
          lastOperation = Pc;
          storeInDataBuffer(message);
        } else {
          Serial.print(F("1:Fail\n"));
        }
      }
      break;
    case 2:
      { // retrive entry
        sendToBluetooth(bluetoothSerial, inputString); //here no need of storing because for retriving is need it.
      }
      break;
    case 3:
      { // delete entry - no need to process, send directly to phone
        lastOperation = Pc;
        storeInDataBuffer(inputString);
      }
      break;
    case 4:
      { // obtain websites - no need to process, send directly to phone
        lastOperation = Pc;
        storeInDataBuffer(inputString);
      }
      break;
    case 5:
      {
        Serial.print(F("5\rCorrect Port\n"));
        break;
      }
    case 7:
      {
        if (generatePassword(inputString, password, KEY_SIZE)) {
          if (encryptPassword(password, key, KEY_SIZE, encryptedPassword)) {
            generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
            lastOperation = Pc;
            storeInDataBuffer(message);
            Serial.print(F("7\rStored in Buffer\n"));
          } else {
            Serial.print(F("7\rFail\n"));
          }
        } else {
          Serial.print(F("7\rFail\n"));
       }
        break;
      }
    case 8:
      {
        Serial.print(F("8\rConnected\n"));
      }
    default:
      break;   
  }
}

void bluetoothProcessReply(SoftwareSerial* bluetoothSerial, char *inputString)
{
  char encryptedPassword[2 * PASSWORD_SIZE]; //make them more local
  char shortEncryptedPassword[PASSWORD_SIZE];
  char password[PASSWORD_SIZE];
  char message[MESSAGE_SIZE];
  
  memset(encryptedPassword, 0, 2 * PASSWORD_SIZE);
  memset(shortEncryptedPassword, 0, PASSWORD_SIZE);
  memset(password, 0, PASSWORD_SIZE);
  memset(message, 0, MESSAGE_SIZE);
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      { // add new entry
        sendToSerial(inputString);
      }
      break;
    case 2:
      { // retrive entry
        getLastMessage(inputString, encryptedPassword);
        generateShortPassword(encryptedPassword, shortEncryptedPassword);

        if (decryptPassword(shortEncryptedPassword, key, KEY_SIZE, password)) {
          generateSerialRetriveMessage(password, message);
          lastOperation = Phone;
          storeInDataBuffer(message);       
        }
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
    case 9:
      {
        sendToBluetooth(bluetoothSeria, "9\n");
      }
    default:
      // error, so ignore data
      break;   
  }
}
