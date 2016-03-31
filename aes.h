#ifndef _AES_H_
#define _AES_H_

#include <Arduino.h>

bool encryptPassword(char* password, char *key, byte KEY_SIZE, char* encryptedPassword);
bool decryptPassword(char* encryptedPassword, char *key, byte KEY_SIZE, char* password);

#endif
