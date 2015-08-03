#include "utils.h"

/*
Make safe the strlen, is possible that the char* not to have a '\0' 
therefore it will never find the end.
*/

/*
Password should be store somewhere it would be needed for decryption
NOT, I have to say again the key which to use for decryption
*/

const char SPLITTER = ':';

int getTypeCommand(const char* message) {
  int lengthString = strlen(message);
  if ( lengthString < 1 ) {
    return -1;
  }
  int type = message[0] - '0';
  return type;
}

void getPassword(const char* message, char* password) {
  // get the 3rd occurence of SPLITTER
  const char* p = strchr(message, SPLITTER);
  p = strchr(p+1,SPLITTER);
  p = strchr(p+1,SPLITTER);
  
  // get the last occurence of SPLITTER
  const char* lastSymbol = strrchr(message, SPLITTER);
  
  // copy in password the string between SPLITTER;
  strncpy(password, p+1, lastSymbol - p -1); //+1 to ignore the SPLITTER, length should be the distance between the symbols - 1 because of the SPLITTER
}

void getEncryptedPassword(const char* message, char *encryptedPassword) {
  const char* p = strchr(message,SPLITTER);
  strncpy(encryptedPassword, p+1, strlen(p)-2);
}

void getKey(const char* message, char* key) {
  // get the last occurence of SPLITTER
  const char* lastSymbol = strrchr(message, SPLITTER);
  
  // copy in key the string from the symbol to the end
  strncpy(key, lastSymbol+1, strlen(lastSymbol)-2); //+1 to ignore the SPLITTER, -2 to ignore the '\n' and SPLITTER
}

void generateBluetoothAddMessage(const char* input, const char* password, char* message)
{
  // get the 3rd occurence of ':'
  const char* p = strchr(input, SPLITTER);
  p = strchr(p+1, SPLITTER);
  p = strchr(p+1, SPLITTER);
  
  int sizeData = p - input;
  strncpy(message, input, sizeData); //copies the code, website and username in message
  message[sizeData] = SPLITTER;
  //strncpy(&message[sizeData+1], password, 16); //copies the password in message
  //message[sizeData+16+1] = '\n';
}

void generateBluetoothRetriveMessage(const char* input, char *message)
{
  const char *p = strrchr(input,SPLITTER);
  strncpy(message, input, p-input);
  message[p-input] = '\n';
}

void generateSerialRetriveMessage(const char* password, char* message)
{
  strncpy(message,"2:",2);
  strncpy(&message[2], password, 16);
  message[2 + 16] = '\n';
}

unsigned char convertToHex( char data ) {
      	switch (data)
      	{
      		case '0': return 0x0;
      		case '1': return 0x1;
      		case '2': return 0x2;
      		case '3': return 0x3;
      		case '4': return 0x4;
      		case '5': return 0x5;
      		case '6': return 0x6;
      		case '7': return 0x7;
      		case '8': return 0x8;
      		case '9': return 0x9;
      		case 'A': return 10;
      		case 'a': return 10;
      		case 'B': return 11;
      		case 'b': return 11;
      		case 'C': return 12;
      		case 'c': return 12;
      		case 'D': return 13;
      		case 'd': return 13;
      		case 'E': return 14;
      		case 'e': return 14;
      		case 'F': return 15;
      		case 'f': return 15;
                default: return 0x0;
      	}
}


