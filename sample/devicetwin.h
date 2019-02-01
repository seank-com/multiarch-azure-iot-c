#ifndef DEVICETWIN_H
#define DEVICETWIN_H

#include <inttypes.h>
#include "iothub_device_client.h"

typedef struct TWIN_SETTINGS_TAG
{
  char* softwareVersion;
  uint32_t telemetryCadence;
} TwinSettings;

extern TwinSettings g_settings;

void DeviceTwin_Init(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection);
void DeviceTwin_Deinit();

#endif
