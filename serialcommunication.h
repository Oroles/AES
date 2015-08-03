#ifndef _SERIAL_COMMUNICATION_H_
#define _SERIAL_COMMUNICATION_H_

#include <SoftwareSerial.h>

void sendSerialReply(char *message);
void serialProcessRequest(SoftwareSerial* serial, char* message);
void sendBluetoothRequest(SoftwareSerial* serial, char* command);
void bluetoothProcessReply(char *command);

#endif
