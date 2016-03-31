#ifndef _PASSWORD_GENERATOR_H_
#define _PASSWORD_GENERATOR_H_

//bool extractInformation(char* message, bool *allow_types, int &l);
//void generatePassword(const bool *allow_types, char *password, const int l);

bool generatePassword(char *message, char* password, int PASSWORD_SIZE);

void generatePasswordImpl(const bool* allow_types, char* password, const int l);

#endif
