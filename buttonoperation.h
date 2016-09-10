#ifndef _BUTTON_OPERATION_H_
#define _BUTTON_OPERATION_H_

#include <SoftwareSerial.h>
#include <Arduino.h>

enum MessageReceiver{ Pc, Phone, None };

void storeInDataBuffer( char * message );
void sendDataFromBuffers(SoftwareSerial* bluetoothSerial, int buttonStatus);
void sendToBluetooth(SoftwareSerial* serial, const char* message);

void setMessageReceiver(MessageReceiver receiver);

void setEnableBluetoothOperations(boolean enable);
boolean getEnableBluetoothOperations();

#endif
