#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include "passwordgenerator.h"
#include <EEPROM.h>
#include "buttonoperation.h"
#include "memoryaccess.h"

char key[KEY_SIZE];

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
  Serial.print(typeCommand);
  switch (typeCommand)
  {
    case '0' + 1:
      { // add new entry
        getLastMessage(inputString, password);
        if (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword)) {
          generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
          setMessageReceiver(Phone);
          storeInDataBuffer(message);
        }
        else {
          generateErrorMessage(message);
          sendToBluetooth(bluetoothSerial, message);
        }
      }
      break;
    case '0' + 2:
      { // retrive entry
        getLastMessage(inputString, encryptedPassword);
        generateShortPassword(encryptedPassword, shortEncryptedPassword);

        if (decryptPassword((unsigned char*)shortEncryptedPassword, (unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)password)) {
          generateSerialRetriveMessage(password, message);
          setMessageReceiver(Pc);
          storeInDataBuffer(message);
        }
        else {
          generateErrorMessage(message);
          sendToBluetooth(bluetoothSerial, message);
        }
      }
      break;
    case '0' + 5:
      { // get from message and store the hash value and close connection
        char hash[HASH_SIZE]; //input = "5:hash_value\n
        memset(hash, '\0', HASH_SIZE);
        getLastMessage(inputString, hash);
        writeHash(hash);
      }
      break;
    case '0' + 6:
      { //read hash from EERPOM and send back to bluetooth
        char hash[HASH_SIZE]; //input = 6:\n
        memset(hash, '\0', HASH_SIZE);
        readHash(hash);
        generateBluetoothRetrieveHash(hash, HASH_SIZE, message); //message = 6:hash_value\n
        storeInDataBuffer(message);
        setMessageReceiver(Phone);
        setEnableBluetoothOperations(true);
      }
      break;
    case '0' + 7:
      { // generate password
        Serial.println("Generate Password");
        Serial.print(inputString);
        Serial.flush();
        if ((generatePassword(inputString, password, PASSWORD_CHUNCKS)) &&
           (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword))) {
            Serial.println("Password Generated");
            generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
            setMessageReceiver(Phone);
            storeInDataBuffer(message);
            memset(message, '\0', MESSAGE_SIZE);
            generateStoredInBuffer(message);
            sendToBluetooth(bluetoothSerial, message);
            Serial.println("Everything is perfect");
        } else {
          generateErrorMessage(message);
          sendToBluetooth(bluetoothSerial, message);
        }
      }
      break;
    case '0' + 8:
      { // get the salt
        char salt[SALT_SIZE];
        memset(salt, '\0', SALT_SIZE);
        getLastMessage(inputString, salt);
        readKey(key, KEY_SIZE, salt);
      }
      break;
    case '0' + 9:
      { // is alive message
        generateIsAliveMessage(message);
        sendToBluetooth(bluetoothSerial, message);
      }
      break;
    default:
      {
        generateErrorMessage(message);
        sendToBluetooth(bluetoothSerial, message);
      }
      break;   
  }
}
