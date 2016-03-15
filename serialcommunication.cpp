#include "serialcommunication.h"
#include "utils.h"
#include "aes.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Keyboard.h"

static const byte KEY_SIZE = 16;
static const byte PASSWORD_SIZE = 100;
static const byte MESSAGE_SIZE = 200;
static const int START_PASSWORD = 900;

static char key[KEY_SIZE];

void readKey() {
  for ( int i = 0; i < KEY_SIZE; ++i ) {
    /*key[i] = ( i % 8 ) + '0';*/
    key[i] = EEPROM.read(i + START_PASSWORD);
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

void sendSerialReply(char *message)
{
  int i = 0;
  while(message[i] != '\0') {
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

/*void debug(char *message, int l)
{
  int i =0;
  for(i=0; i<l; ++i) {
    if ((unsigned char) message[i] < 0x10 ) {
      Serial.print(0x00, HEX);
    }
    Serial.print((unsigned char)message[i], HEX);
  }
  Serial.print("\n");
}*/

void serialProcessRequest(SoftwareSerial* serial, char* inputString)
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
        sendBluetoothRequest(serial, message);
        sendBluetoothRequest(serial, encryptedPassword, strlen(password));
        serial->println("");

      } else {
        Serial.print(F("1:Fail\n")); //remove hardcoded part
      }

      break;
    case 2:
      // retrive entry
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

  unsigned long expansionKey[44];
  memset(expansionKey, 0, 44 * 4);
  
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

      // process key
      keyExpansion(key, expansionKey);

      // process password
      getLastMessage(inputString, encryptedPassword);
      // for an unknown reason read one more char so I have to remove it
      encryptedPassword[ KEY_SIZE * (strlen(encryptedPassword) / KEY_SIZE)] = '\0';
      generateShortPassword(encryptedPassword, shortEncryptedPassword);

      if ( strlen(encryptedPassword) % ( KEY_SIZE * 2 ) == 0 ) {
        
        // decrypt message
        int steps = strlen(encryptedPassword) / (KEY_SIZE * 2);
        int contor = 0;
        while( contor < steps ) {
          invCipher(&shortEncryptedPassword[contor * KEY_SIZE], expansionKey, &password[contor * KEY_SIZE]);
          ++contor;
        }

        //generateSerialRetriveMessage(inputString, password, message);
        //storeInBuffer(message);
        generateSerialRetriveMessage(password, message);
        storeInBuffer(message);
      } else {
        Serial.print(F("2:0:Fail\n")); //remove hardcoded part
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
      break;
    default:
      // error, so ignore data
      break;   
  }
}
