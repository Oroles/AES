#include "utils.h"
#//include <EEPROM.h>

const char* getNOccurrence( const char* input, int n, const char c) {
  const char* p = input;
  while( n != 0 ) {
    p = strchr(p+1, c);
    if ( p == 0 ) {
      return p;
    }
    --n;
  }
  return p;
}

int getTypeCommand(const char* message) {
  int lengthString = strlen(message);
  if ( lengthString < 1 ) {
    return -1;
  }
  int type = message[0];
  return type;
}

bool getLastMessage(const char* input, char* result) {
  const char* p1 = strrchr(input, SPLITTER);
  const char* p2 = strrchr(input, END_COMMAND);
  if ( (p1 == 0) || (p2 == 0) ) {
    return false;
  }
  strncpy(result, p1+1, p2-p1-1);
  return true;
}

byte getPasswordLength(const char* input) {
  const char* p1 = strchr(input, SPLITTER);
  const char* p2 = strrchr(input, SPLITTER);
  char tmp[10];
  strncpy(tmp, p1+1, p2-p1-1);
  return (byte)getLong(tmp);
}

unsigned char asciiToHex( char data ) {
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

char hexToAscii( char data ) {
  switch (data)
  {
    case 0x00: return '0';
    case 0x01: return '1';
    case 0x02: return '2';
    case 0x03: return '3';
    case 0x04: return '4';
    case 0x05: return '5';
    case 0x06: return '6';
    case 0x07: return '7';
    case 0x08: return '8';
    case 0x09: return '9';
    case 0x0A: return 'A';
    case 0x0B: return 'B';
    case 0x0C: return 'C';
    case 0x0D: return 'D';
    case 0x0E: return 'E';
    case 0x0F: return 'F';
    default: return '0';  
  }
}

long getLong(const char* v) {
  long result = 0;
  sscanf(v, "%ld", &result);
  return result;
}

void generateShortPassword(const char* longPassword, char *password)
{
  unsigned char i = 0;
  for( i = 0; i < strlen(longPassword); i+=2 )
  {
    unsigned char MSB = asciiToHex(longPassword[i]);
    unsigned char LSB = asciiToHex(longPassword[i+1]);
    password[i/2] = ( ( MSB << 4 ) | LSB );
  }
}

void generateBluetoothAddMessage(const char* input, const char* password, int passwordLength, char* message) {
  // get the 3rd occurence of ':'
  const char* p = getNOccurrence(input, 3, SPLITTER);
  
  int sizeData = p - input;
  strncpy(message, input, sizeData + 1); //copies the code, website and username in message
  for ( int i = 0, j = 0; i < passwordLength; ++i, ++j ) {
    char msb = hexToAscii((0xF0 & password[i]) >> 4 );
    char lsb = hexToAscii(0x0F & password[i]);
    message[2 * j + sizeData + 1] = msb;
    message[2 * j + 1 + sizeData + 1] = lsb;
  }
  message[strlen(message)] = '\n';
}

void generateBluetoothAddNote(const char* input, const char* password, int passwordLength, char* message) {
  // get the 2rd occurence of ':'
  const char* p = getNOccurrence(input, 2, SPLITTER);
  
  int sizeData = p - input;
  strncpy(message, input, sizeData + 1); //copies the code, website and username in message
  for ( int i = 0, j = 0; i < passwordLength; ++i, ++j ) {
    char msb = hexToAscii((0xF0 & password[i]) >> 4 );
    char lsb = hexToAscii(0x0F & password[i]);
    message[2 * j + sizeData + 1] = msb;
    message[2 * j + 1 + sizeData + 1] = lsb;
  }
  message[strlen(message)] = '\n';
}

void generateSerialRetriveInfo(const char* password, char* message) {
  const char *p = strchr(password, PADDING);
  strncpy(message, password, p - password);
  message[p-password] = '\0';
}

void generateBluetoothRetrieveHash(const char* hash, int l, char* message) {
  message[0] = '0' + 6;
  message[1] = SPLITTER;
  strncpy(&message[2], hash, l);
  message[2 + l] = '\n';
}

void generateBluetoothLastTimeUsed(const char* lastTimeUsed, int l, char* message) {
  message[0] = '0' + 4;
  message[1] = SPLITTER;
  strncpy(&message[2], lastTimeUsed, l);
  message[2 + l] = '\n';
}

void generateErrorMessage(char* message)
{
  message[0] = '0' + 11;
  memcpy(&message[1], "\rError\n", 7);
}

void generateStoredInBuffer(char* message)
{
  message[0] = '0' + 3;
  memcpy(&message[1], "\rPress the button on device\n", 28);
}

void generateIsAliveMessage(char* message)
{
  message[0] = '0' + 9;
  message[1] = END_COMMAND;
}

