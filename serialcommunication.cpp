#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include <Arduino.h>

static const int KEY_SIZE = 16;
static const int PASSWORD_SIZE = 100;
static const int MESSAGE_SIZE = 200;

static unsigned long expansionKey[44];
static char key[KEY_SIZE];

void readKey() {
  for ( int i = 0; i < KEY_SIZE; ++i ) {
    key[i] = ( i % 8 ) + '0';
  }
}

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

void debug(char *message, int l)
{
  int i =0;
  for(i=0; i<l; ++i) {
    if ((unsigned char) message[i] < 0x10 ) {
      Serial.print(0x00, HEX);
    }
    Serial.print((unsigned char)message[i], HEX);
  }
  Serial.print("\n");
}

void serialProcessRequest(SoftwareSerial* serial, char* inputString)
{
  char password[PASSWORD_SIZE]; //no longer than 16 characters
  char encryptedPassword[PASSWORD_SIZE]; //ecrypted password will be on 16 bytes
  char message[MESSAGE_SIZE]; //message that will be send to the bluetooth
  char retrieveEntryMessage[MESSAGE_SIZE];
  
  memset(password, 0, PASSWORD_SIZE);
  memset(encryptedPassword, 0, PASSWORD_SIZE);
  memset(message, 0, MESSAGE_SIZE);
  memset(retrieveEntryMessage, 0, MESSAGE_SIZE);
  
  int typeCommand = getTypeCommand(inputString);
  switch (typeCommand)
  {
    case 1:
      // add new entry

      // process key
      //getLastMessage(inputString, key);
      keyExpansion(key, expansionKey);
      //memset(key, 0, KEY_SIZE); //remove the key

      // process password
      getLastMessage(inputString, password);
      //password[KEY_SIZE] = '\0';
      //getThirdMessage(inputString, password);

      //Serial.print(password);
      //Serial.print("\n");

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
        sendBluetoothRequest(serial, message);
        sendBluetoothRequest(serial, encryptedPassword, strlen(password));
        serial->println("");

        //debug (encryptedPassword, strlen(password));
      } else {
        Serial.print("1:Fail\n"); //remove hardcoded part
      }

      break;
    case 2:
      // retrive entry

      // process key
      //memset(key, 0, KEY_SIZE);
      //getLastMessage(inputString, key);

      // generate request
      //generateBluetoothRetriveMessage(inputString, retrieveEntryMessage);

      // send message
      //sendBluetoothRequest(serial, retrieveEntryMessage);
      sendBluetoothRequest(serial, inputString);
      break;
    case 3:
      // delete entry - no need to process, send directly to phone
      sendBluetoothRequest(serial, inputString);
      break;
    case 4:
      // obtain websites - no need to process, send directly to phone
      sendBluetoothRequest(serial, inputString);
      break;
    case 5:
      // close bluetooth the connection - no need to process, send directly to phone
      sendBluetoothRequest(serial, inputString);
    default:
      //memset(key, 0, KEY_SIZE);
      // error, so ignore data
      break;   
  }
}

void bluetoothProcessReply(char *inputString)
{
  char encryptedPassword[2 * PASSWORD_SIZE];
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
      // add new entry - ne need to process, send directly to serial
      // the message should be Ok or Fail
      sendSerialReply(inputString);
      break;
    case 2:
      // retrive entry

      // process key - as putea sa tin in memorie expansionkey-ul asa ar fi mai greu de procesat
      keyExpansion(key, expansionKey);
      //memset(key, 0, KEY_SIZE);

      // process password
      getLastMessage(inputString, encryptedPassword);
      // for an unknown reason read one more char so I have to remove it
      encryptedPassword[ KEY_SIZE * (strlen(encryptedPassword) / KEY_SIZE)] = '\0';
      generateShortPassword(encryptedPassword, shortEncryptedPassword);

      //debug(shortEncryptedPassword, strlen(shortEncryptedPassword));
      //debug(key, strlen(key) );
      if ( strlen(encryptedPassword) % ( KEY_SIZE * 2 ) == 0 ) {
        
        // decrypt message
        int steps = strlen(encryptedPassword) / (KEY_SIZE * 2);
        int contor = 0;
        while( contor < steps ) {
          invCipher(&shortEncryptedPassword[contor * KEY_SIZE], expansionKey, &password[contor * KEY_SIZE]);
          ++contor;
        }

        generateSerialRetriveMessage(inputString, password, message);
        sendSerialReply(message);
      } else {
        Serial.print("2:0:Fail\n"); // //remove hardcoded part
      }

      break;
    case 3:
      // delete entry - no need to process, send directly to serial
      sendSerialReply(inputString);
      break;
    case 4:
      // obtain websites - no need to process, send directly to serial
      sendSerialReply(inputString);
      break;
    case 5:
      // close serial connection
      sendSerialReply(inputString);
    default:
      // error, so ignore data
      break;   
  }
}
