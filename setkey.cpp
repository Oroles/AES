#include "setkey.h"
#include "Arduino.h"
#include "utils.h"
#include "EEPROM.h"
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
    Serial.println(F("Start to process the key"));
    unsigned char hashKey[32];
    spongent((unsigned char*)inputSerial, hashKey);

    for (int index = 0; index < 32; ++index)
    {
      EEPROM[index + START_KEY] = hashKey[index];
    }
    Serial.println("");
    state = FINISH;
    
    Serial.println(F("Just unplug it and plug it again to use it"));
  }
}
