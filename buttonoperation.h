#ifndef _BUTTON_OPERATION_H_
#define _BUTTON_OPERATION_H_

#include <SoftwareSerial.h>
#include <Arduino.h>

enum LastOperation{ Pc, Phone, Button, None };

void storeInDataBuffer( char * message );
void sendDataFromBuffers(SoftwareSerial* bluetoothSerial, int buttonStatus);

void sendToSerial(const char *message);
void sendToBluetooth(SoftwareSerial* serial, const char* message);

void setLastOperation(LastOperation op);
void setEnableBluetoothOperations(boolean enable);

boolean getEnableBluetoothOperations();

#endif
