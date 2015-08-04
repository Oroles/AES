#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>

int getTypeCommand(const char* message);
void getPassword(const char* message, char* password);
void getEncryptedPassword(const char* message, char *encryptedPassword);
void getKey(const char* message, char* key);
void generateBluetoothAddMessage(const char* input, const char* password, char* message);
void generateBluetoothRetriveMessage(const char* input, char *message);
void generateSerialRetriveMessage(const char* input, char* password);
void generateShortPassword(const char* longPassword, char *password);

#endif
