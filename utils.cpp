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
  strncpy(encryptedPassword, &message[2], 32);
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
    case '0': return 0x00;
    case '1': return 0x01;
    case '2': return 0x02;
    case '3': return 0x03;
    case '4': return 0x04;
    case '5': return 0x05;
    case '6': return 0x06;
    case '7': return 0x07;
    case '8': return 0x08;
    case '9': return 0x09;
    case 'A': return 0x0A;
    case 'a': return 0x0A;
    case 'B': return 0x0B;
    case 'b': return 0x0B;
    case 'C': return 0x0C;
    case 'c': return 0x0C;
    case 'D': return 0x0D;
    case 'd': return 0x0D;
    case 'E': return 0x0E;
    case 'e': return 0x0E;
    case 'F': return 0x0F;
    case 'f': return 0x0F;
    default: return 0x00;
  }
}

void generateShortPassword(const char* longPassword, char *password)
{
  int i = 0;
  for( i = 0; i < 32; i+=2 )
  {
    unsigned char MSB = convertToHex(longPassword[i]);
    unsigned char LSB = convertToHex(longPassword[i+1]);
    password[i/2] = ( ( MSB << 4 ) | LSB );
  }
}


