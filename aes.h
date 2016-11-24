#ifndef _AES_H_
#define _AES_H_

#include <Arduino.h>

bool encryptPassword(const unsigned char* password,const unsigned char *key, byte KEY_SIZE,unsigned char* encryptedPassword);
bool decryptPassword(const unsigned char* encryptedPassword,const unsigned char *key, byte KEY_SIZE, byte l, unsigned char* password);

#endif
