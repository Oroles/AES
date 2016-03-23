#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>

const char SPLITTER = '\r';
const char END_COMMAND = '\n';
const char PADDING = '\t';

int getTypeCommand(const char* message);
void generateBluetoothAddMessage(const char* input, char* message);
void generateShortPassword(const char* longPassword, char *password);
void generateSerialRetriveMessage(const char* password, char* message);
void generateBluetoothRetrieveHash(const char* hash, int l, char* message);
void generateSerialClose(char* message);

bool getLastMessage(const char* input, char* result);

#endif
