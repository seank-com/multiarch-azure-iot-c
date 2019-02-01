#ifndef SHARED_H
#define SHARED_H

#include "iothub_device_client.h"

#define SHARED_VALUE_UNCHANGED 0
#define SHARED_VALUE_CHANGED 1

void Shared_Init(char* softwareVersion, unsigned int defaultTelemetryCadence);
void Shared_Deinit();

IOTHUB_CLIENT_CONNECTION_STATUS Shared_GetStatus();
int Shared_SetStatus(IOTHUB_CLIENT_CONNECTION_STATUS status);

const char* Shared_GetVersion();

unsigned int Shared_GetTelemetryCadence();
int Shared_SetTelemetryCadence(unsigned int telemetryCadence);

#endif