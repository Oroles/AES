#ifndef _MEMORY_ACCESS_H_
#define _MEMORY_ACCESS_H_

#include <Arduino.h>

void readHash(char* hash);
void writeHash(char* hash);

void readLastTimeUsed(char* hash);
void writeLastTimeUsed(char* hash);

void readKey(char* key, byte keySize, const char* salt);

#endif
