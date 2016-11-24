#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include "passwordgenerator.h"
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
  
  switch (typeCommand)
  {
    case '0' + 1:
      { // add new entry
        getLastMessage(inputString, password);
        if (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword)) {
          generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
          setMessageReceiver(Phone);
          storeInDataBuffer(message);
          memset(message, '\0', MESSAGE_SIZE);
          generateStoredInBuffer(message);
          sendToBluetooth(bluetoothSerial, message);
        }
        else {
          generateErrorMessage(message);
          sendToBluetooth(bluetoothSerial, message);
        }
      }
      break;
    case '0' + 2: //retrive password
    case '0' + 13: //retrive note 
      {
        getLastMessage(inputString, encryptedPassword);
        generateShortPassword(encryptedPassword, shortEncryptedPassword);
        byte l = getPasswordLength(inputString);
        if (decryptPassword((unsigned char*)shortEncryptedPassword, (unsigned char*)key, PASSWORD_CHUNCKS, l, (unsigned char*)password)) {
          generateSerialRetriveInfo(password, message);
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
     case '0' + 4:
      {
        char lastTimeUsed[LAST_TIME_USED_SIZE];
        memset(lastTimeUsed, '\0', LAST_TIME_USED_SIZE);
        readLastTimeUsed(lastTimeUsed);
        generateBluetoothLastTimeUsed(lastTimeUsed, LAST_TIME_USED_SIZE, message);
        //Serial.print(message);
        sendToBluetooth(bluetoothSerial, message);
      }
      break;
    case '0' + 10:
      {
        char lastTimeUsed[LAST_TIME_USED_SIZE];
        memset(lastTimeUsed, '\0', LAST_TIME_USED_SIZE);
        getLastMessage(inputString, lastTimeUsed);
        writeLastTimeUsed(lastTimeUsed);
      }
      break;
    
    case '0' + 7:
      { // generate password
        if ((generatePassword(inputString, password, PASSWORD_CHUNCKS)) &&
           (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword))) {
            generateBluetoothAddMessage(inputString, encryptedPassword, strlen(password), message);
            setMessageReceiver(Phone);
            storeInDataBuffer(message);
            memset(message, '\0', MESSAGE_SIZE);
            generateStoredInBuffer(message);
            sendToBluetooth(bluetoothSerial, message);
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
    case '0' + 12:
      { //add note
        getLastMessage(inputString, password);//password = note text
        if (encryptPassword((const unsigned char*)password, (const unsigned char*)key, PASSWORD_CHUNCKS, (unsigned char*)encryptedPassword)) {
          generateBluetoothAddNote(inputString, encryptedPassword, strlen(password), message);
          setMessageReceiver(Phone);
          storeInDataBuffer(message);
          
          generateStoredInBuffer(message);
          sendToBluetooth(bluetoothSerial, message);
        }
        else {
          generateErrorMessage(message);
          sendToBluetooth(bluetoothSerial, message);
        }
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
