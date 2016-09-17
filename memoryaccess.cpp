#include "memoryaccess.h"
#include "utils.h"

#include <EEPROM.h>

void readHash(char* hash) {
  for( int i = 0; i < HASH_SIZE; ++i ) {
    hash[i] = EEPROM.read(i + START_HASH);
  }
}

void writeHash(char* hash) {
  for ( int i = 0; i < HASH_SIZE; ++i ) {
    EEPROM.write(i + START_HASH, hash[i]);
  }
}

void readLastTimeUsed(char* lastTimeUsed) {
  for (int i = 0; i < LAST_TIME_USED_SIZE; ++i) {
    lastTimeUsed[i] = EEPROM.read(i + START_LAST_TIME_USED);
  }
}

void writeLastTimeUsed(char* lastTimeUsed) {
  for (int i = 0; i < LAST_TIME_USED_SIZE; ++i) {
    if (lastTimeUsed[i] != '\0')
      EEPROM.write(i + START_LAST_TIME_USED, lastTimeUsed[i]);
    else
      EEPROM.write(i + START_LAST_TIME_USED, '\t');
  }
}

void swap(char* a, char* b) {
  char t = *a;
  *a = *b;
  *b = t;
}

void deshuffle(char* message, byte l, long n) {
  int i = l-1;
  while ( i > 0) {
    if (n & 0x40000000) { //the SIZE bit is set
      swap(&message[0], &message[i]);
    }
    --i;
    n <<= 1;
  }
}

void readKey(char* key, byte l) {
  for ( int i = 0; i < l; ++i ) {
    key[i] = EEPROM.read(i + START_KEY);
  }
}

void readKey(char* key, byte keySize, const char* salt) {
  long value = getLong(salt);
  readKey(key, keySize);
  deshuffle(key, keySize, value);
}

