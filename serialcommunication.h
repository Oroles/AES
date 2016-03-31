#ifndef _SERIAL_COMMUNICATION_H_
#define _SERIAL_COMMUNICATION_H_

#include <SoftwareSerial.h>
#include <Entropy.h>

void serialProcessRequest(SoftwareSerial* bluetoothSerial, char* message);
void bluetoothProcessReply(SoftwareSerial* bluetootSerial, char *command);
void readKey();
void sendPasswordAsKeyboard(int buttonStatus);

#endif
