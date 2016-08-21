#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <Arduino.h>

const char SPLITTER = '\r';
const char END_COMMAND = '\n';
const char PADDING = '\t';

const byte KEY_SIZE = 32;
const byte SALT_SIZE = 32;
const byte PASSWORD_CHUNCKS = 16;
const byte HASH_SIZE = 32;
const byte PASSWORD_SIZE = 100;
const byte MESSAGE_SIZE = 200;
const int START_KEY = 900;
const int START_HASH = 950;

int getTypeCommand(const char* message);
void generateBluetoothAddMessage(const char* input, const char* password, int passwordLength, char* message);
void generateShortPassword(const char* longPassword, char *password);
void generateSerialRetriveMessage(const char* password, char* message);
void generateBluetoothRetrieveHash(const char* hash, int l, char* message);
void generateSerialClose(char* message);

bool getLastMessage(const char* input, char* result);
const char* getNOccurrence( const char* input, int n, const char c);

#endif
