#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include "passwordgenerator.h"
#include <EEPROM.h>
#include "buttonoperation.h"
#include "memoryaccess.h"

char key[KEY_SIZE];

/*void readKey() {
  for ( int i = 0; i < KEY_SIZE; ++i ) {
    key[i] = EEPROM.read(i + START_KEY);
  }
}*/

/*void readHash(char* hash) {
  for( int i = 0; i < HASH_SIZE; ++i ) {
    hash[i] = EEPROM.read(i + START_HASH);
  }
}

void writeHash(char* hash) {
  for ( int i = 0; i < HASH_SIZE; ++i ) {
    EEPROM.write(i + START_HASH, hash[i]);
  }
}*/

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
        if (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword)) {
          generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
          setLastOperation(Pc);
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
        setLastOperation(Pc);
        storeInDataBuffer(inputString);
      }
      break;
    case 4:
      { // obtain websites - no need to process, send directly to phone
        setLastOperation(Pc);
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
        if (generatePassword(inputString, password, PASSWORD_CHUNCKS)) {
          if (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword)) {
            generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
            setLastOperation(Pc);
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
        break;
      }
    default:
      {
        Serial.print(F("9\rError\n"));
      }
      break;   
  }
}

/*void displayMessage(char* message, byte KEY_SIZE) {
  Serial.println("--------------------------------");
  for (int index = 0; index < KEY_SIZE; ++index) {
    Serial.print(message[index], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}*/

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

  /*if (typeCommand != 6 && getEnableBluetoothOperations() == false)
  {
    return;
  }*/
  
  switch (typeCommand)
  {
    case 1:
      { // add new entry
        sendToSerial(inputString);
      }
      break;
    case 2:
      { // retrive entry
        Serial.print("Received send password request");
        getLastMessage(inputString, encryptedPassword);
        generateShortPassword(encryptedPassword, shortEncryptedPassword);

        if (decryptPassword((unsigned char*)shortEncryptedPassword, (unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)password)) {
          generateSerialRetriveMessage(password, message);
          setLastOperation(Phone);
          storeInDataBuffer(message);
        }
        else {
          sendToBluetooth(bluetoothSerial, "10\rError\n");
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
        storeInDataBuffer(message);
        setLastOperation(Button);
      }
      break;
    case 7:
      { // generate password
        sendToSerial(inputString);
      }
      break;
    case 8:
      { // get the salt
        char salt[SALT_SIZE];
        memset(salt, '\0', SALT_SIZE);
        getLastMessage(inputString, salt);
        readKey(key, KEY_SIZE, salt);
      }
      break;
    case 9:
      { // is alive message
        sendToBluetooth(bluetoothSerial, "9\n");
      }
      break;
    default:
      {
        sendToBluetooth(bluetoothSerial, "10\rError\n");
      }
      break;   
  }
}
