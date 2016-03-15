#include "utils.h"

/*
Make safe the strlen, is possible that the char* not to have a '\0' 
therefore it will never find the end.
*/

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
  int type = message[0] - '0';
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

void generateBluetoothAddMessage(const char* input, char* message)
{
  // get the 3rd occurence of ':'
  const char* p = getNOccurrence(input, 3, SPLITTER);
  
  int sizeData = p - input;
  strncpy(message, input, sizeData + 1); //copies the code, website and username in message
}

void generateBluetoothRetriveMessage(const char* input, char *message)
{
  const char *p = strrchr(input,SPLITTER);
  strncpy(message, input, p-input);
  message[p-input] = END_COMMAND;
}

void generateSerialRetriveMessage(const char* input, const char* password, char* message)
{
  strncpy(message, input, 4); //will copy "2:status:"
  strncpy(&message[4], password, strlen(password));
  message[4 + strlen(password)] = '\n';
}

void generateSerialRetriveMessage(const char* password, char* message)
{
  const char *p = strchr(password, PADDING);
  strncpy(message, password, p - password);
  message[p-password] = '\0';
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
  unsigned char i = 0;
  for( i = 0; i < strlen(longPassword); i+=2 )
  {
    unsigned char MSB = convertToHex(longPassword[i]);
    unsigned char LSB = convertToHex(longPassword[i+1]);
    password[i/2] = ( ( MSB << 4 ) | LSB );
  }
}

