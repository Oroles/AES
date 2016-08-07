#include "passwordgenerator.h"
#include "utils.h"

#include <Entropy.h>
#include <stdio.h>

const int SYMBOL_SIZE = 20;
static char symbols[SYMBOL_SIZE] = {'!','$','%','^','&','*','(',')','~','/',',','.','[',']',';','\\','|',':','{','}' };

const int SMALL_LETTERS_LOWER = 97;
const int SMALL_LETTERS_UPPER = 122;

const int CAPITAL_LETTERS_LOWER = 65;
const int CAPITAL_LETTERS_UPPER = 90;

const int NUMBERS_LOWER = 48;
const int NUMBERS_UPPER = 57;

const int PASSWORD_LENGTH = 16;

const int TYPES_SIZE = 3;
enum TYPES {
    SYMBOLS = 0,
    NUMBERS = 1,
    LETTERS = 2
};

bool isLetter(char c) {
  if ((SMALL_LETTERS_LOWER <= c && c < SMALL_LETTERS_UPPER) ||
     (CAPITAL_LETTERS_LOWER <=c && c < CAPITAL_LETTERS_UPPER)) {
    return true;    
  }
  return false;
}

bool isNumber(char c) {
  if (NUMBERS_LOWER <= c && c < NUMBERS_UPPER) {
    return true;
  }
  return false;
}

bool isSymbol(char c) {
  for (int i = 0; i < SYMBOL_SIZE; ++i) {
    if (c == symbols[i]) {
      return true;
    }
  }
  return false;
}

bool isValid(const char* password, const int l, const bool* types) {
  char tmp[TYPES_SIZE] = {0,0,0}; 
  for (int i = 0; i < l; ++i) {
    char c = password[i];
    if (isLetter(c) && types[LETTERS] == true) {
      tmp[LETTERS] = 1;
    }
    if (isNumber(c) && types[NUMBERS] == true) {
      tmp[NUMBERS] = 1;
    }
    if (isSymbol(c) && types[SYMBOLS] == true) {
      tmp[SYMBOLS] = 1;
    }
  }
  for (int i = 0; i < TYPES_SIZE; ++i) {
    if (types[i] == true && tmp[i] != 1) {
      return false;
    }
  }
  return true;
}

void obtainCounters(const bool* allow_types, int l, int *counterSymbols, int *counterNumbers, int *counterLetters)
{
  *counterSymbols = 0;
  *counterNumbers = 0;
  *counterLetters = 0;
  
  if (allow_types[SYMBOLS] == true && (allow_types[NUMBERS] == true || allow_types[LETTERS] == true)) {
    *counterSymbols = Entropy.random(l - allow_types[NUMBERS] - allow_types[LETTERS]);
  }
  else {
    if (allow_types[NUMBERS] == false && allow_types[LETTERS] == false) {
        *counterSymbols = l;
    }
  }
  
  if (allow_types[NUMBERS] == true && allow_types[LETTERS] == true) {
    *counterNumbers = Entropy.random(l - *counterSymbols - allow_types[LETTERS]);
  }
  else {
    if (allow_types[LETTERS] == false) {
      *counterNumbers = l - *counterSymbols;
    }
  }
  
  if (allow_types[LETTERS] == true) {
    *counterLetters = l - *counterNumbers - *counterSymbols;
  }
}

void randomShuffel(char *password, const int l) {
  for (int i = l-1; i > 0; --i) {
    int j = Entropy.random(0,i-1);
    char tmp = password[j];
    password[j] = password[i];
    password[i] = tmp;
  }
  
}

void generatePasswordImpl2(const bool* allow_types, char* password, const int l) {
  Entropy.initialize();
  int counterSymbols = 0;
  int counterNumbers = 0;
  int counterLetters = 0;
  obtainCounters(allow_types, l, &counterSymbols, &counterNumbers, &counterLetters);
  int counter = 0;
  for (int i = 0; i < counterSymbols; ++i) {
    password[counter++] = symbols[(Entropy.random(SYMBOL_SIZE))];
  }
  for (int i = 0; i < counterNumbers; ++i) {
    password[counter++] = Entropy.random(NUMBERS_LOWER, NUMBERS_UPPER);
  }
  for (int i = 0; i < counterLetters; ++i) {
      if (Entropy.random(2) == 0) {
        password[counter++] = Entropy.random(SMALL_LETTERS_LOWER, SMALL_LETTERS_UPPER);
      }
      else {
        password[counter++] = Entropy.random(CAPITAL_LETTERS_LOWER, CAPITAL_LETTERS_UPPER);
      }
  }
  if ( (allow_types[SYMBOLS] == true && allow_types[NUMBERS] == true) ||
       (allow_types[SYMBOLS] == true && allow_types[LETTERS] == true) ||
       (allow_types[LETTERS] == true && allow_types[NUMBERS] == true) ) {
    randomShuffel(password, l);
  }
  
}

void generatePasswordImpl(const bool* allow_types, char* password, const int l) {
  Entropy.initialize();
  int counter = 0;
  while ( counter < l ) {
    uint8_t category = Entropy.random(3);
    
    if (category == SYMBOLS && allow_types[category] == true) {
      password[counter++] = symbols[(Entropy.random(SYMBOL_SIZE))];
    }
    
    if (category == NUMBERS && allow_types[category] == true) {
      password[counter++] = Entropy.random(NUMBERS_LOWER, NUMBERS_UPPER);
    }
    
    if (category == LETTERS && allow_types[category] == true) {
      if (Entropy.random(2) == 0) {
        password[counter++] = Entropy.random(SMALL_LETTERS_LOWER, SMALL_LETTERS_UPPER);
      }
      else {
        password[counter++] = Entropy.random(CAPITAL_LETTERS_LOWER, CAPITAL_LETTERS_UPPER);
      }
    }
  }
}

void generatePassword(const bool *allow_types, char *password, const int l) {
  do {
    generatePasswordImpl2(allow_types, password, l);
  } while(!isValid(password, l, allow_types ));
}

bool extractTypes(const uint8_t type, bool *allow_types) {
  switch (type) {
    case 0x31: allow_types[SYMBOLS] = false; allow_types[NUMBERS] = false; allow_types[LETTERS] = true; return true;
    case 0x32: allow_types[SYMBOLS] = false; allow_types[NUMBERS] = true; allow_types[LETTERS] = false; return true;
    case 0x33: allow_types[SYMBOLS] = false; allow_types[NUMBERS] = true; allow_types[LETTERS] = true; return true;
    case 0x34: allow_types[SYMBOLS] = true; allow_types[NUMBERS] = false; allow_types[LETTERS] = false; return true;
    case 0x35: allow_types[SYMBOLS] = true; allow_types[NUMBERS] = false; allow_types[LETTERS] = true; return true;
    case 0x36: allow_types[SYMBOLS] = true; allow_types[NUMBERS] = true; allow_types[LETTERS] = false; return true;
    case 0x37: allow_types[SYMBOLS] = true; allow_types[NUMBERS] = true; allow_types[LETTERS] = true; return true;
    default:
      return false;
  }
}

bool extractInformation(char* message, bool *allow_types, int &l) {
  //message = "6\rwebsite\rusername\rtypes\rlength\n"
  const char* p = getNOccurrence(message, 3, SPLITTER);
  if (!extractTypes(*(p+1), allow_types)) {
    return false;
  }
  
  char tmp[10];
  memset(tmp, '\0', 10);
  if (!getLastMessage(message, tmp)) {
    return false;
  }
  
  sscanf(tmp, "%d", &l); //%2d
  if (l > 16 ) {
    return false;
  }
  return true;
}

bool generatePassword(char *message, char* password, int PASSWORD_SIZE) {
  bool allow_types[3];
  int l = 0;
  memset(password, PADDING, PASSWORD_SIZE); //add the padding
  if (extractInformation(message, allow_types, l)) {
    generatePassword(allow_types, password, l);
    return true;
  }
  return false;
}

