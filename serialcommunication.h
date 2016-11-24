#ifndef _SERIAL_COMMUNICATION_H_
#define _SERIAL_COMMUNICATION_H_

#include <SoftwareSerial.h>
#include <Entropy.h>

void bluetoothProcessReply(SoftwareSerial* bluetootSerial, char *command);
void sendDataFromBuffers(SoftwareSerial* bluetoothSerial, int buttonStatus);

#endif
