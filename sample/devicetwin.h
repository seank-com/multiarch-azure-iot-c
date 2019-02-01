#ifndef DEVICETWIN_H
#define DEVICETWIN_H

#include "iothub_device_client.h"

typedef struct TWIN_SETTINGS_TAG
{
  char* softwareVersion;
  unsigned int telemetryCadence;
} TwinSettings;

void DeviceTwin_Init(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection);
void DeviceTwin_Deinit();

TwinSettings* DeviceTwin_CreateTwinSettings(char* softwareVersion, unsigned int telemetryCadence);
void DeviceTwin_DeleteTwinSettings(TwinSettings* settings);

#endif
