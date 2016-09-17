#include "setkey.h"
#include "Arduino.h"
#include "utils.h"
#include "EEPROM.h"
#include "md5.h"
#include "spongent.h"

enum STATE { READ_KEY, PROCESS_KEY, FINISH };
static STATE state = READ_KEY;

int sizeInputSerial = 0;

void readInput(char* inputSerial) {
  if ( state == READ_KEY ) {
    while (Serial.available())
    {
      char c = (char)Serial.read();
      if ( c != '\n' )
      {
        inputSerial[sizeInputSerial++] = c;
        if (sizeInputSerial > 32)
        {
          sizeInputSerial = 0;
        }  
      } else {
        state = PROCESS_KEY;
      }
    }
  }
}

void processKey(char* inputSerial, int l) {
  if (state == PROCESS_KEY) {
    //unsigned char* hash = MD5::make_hash(inputSerial);
    //char* md5str = MD5::make_digest(hash, 32);
    //unsigned char md5str[32];
    Serial.println(F("Start to process the key"));
    unsigned char hashKey[32];
    spongent((unsigned char*)inputSerial, hashKey);
    
    //free(hash);
    
    /*for (int i = 0; i < 32; ++i)
    {
      hashKey[i] = (( asciiToHex(md5str[2*i]) << 4) | asciiToHex(md5str[2*i+1]) );
    }*/

    //Serial.println("Key after md5");
    for (int index = 0; index < 32; ++index)
    {
      EEPROM[index + START_KEY] = hashKey[index];
      //Serial.print(hashKey[index], HEX);
      //Serial.print(" ");
    }
    Serial.println("");
    state = FINISH;
    
    //clearMemory(inputSerial, l);
    Serial.println(F("Just unplug it and plug it again to use it"));
  }
}
