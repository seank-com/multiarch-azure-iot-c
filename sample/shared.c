#include "shared.h"
#include "devicetwin.h"

static IOTHUB_CLIENT_CONNECTION_STATUS s_status = IOTHUB_CLIENT_CONNECTION_AUTHENTICATED;
static TwinSettings* s_settings = NULL;

void Shared_Init(char* softwareVersion, unsigned int defaultTelemetryCadence)
{
  s_settings = DeviceTwin_CreateTwinSettings(softwareVersion, defaultTelemetryCadence);
}

void Shared_Deinit()
{
  DeviceTwin_DeleteTwinSettings(s_settings);
}

IOTHUB_CLIENT_CONNECTION_STATUS Shared_GetStatus()
{
  return s_status;
}

int Shared_SetStatus(IOTHUB_CLIENT_CONNECTION_STATUS status)
{
  if (s_status != status)
  {
    s_status = status;
    return SHARED_VALUE_CHANGED;
  }
  return SHARED_VALUE_UNCHANGED;
}

const char* Shared_GetVersion()
{
  if (s_settings != NULL)
  {
    return s_settings->softwareVersion;
  }
  return NULL;
}

unsigned int Shared_GetTelemetryCadence()
{
  if (s_settings != NULL)
  {
    return s_settings->telemetryCadence;
  }
  return 0;
}

int Shared_SetTelemetryCadence(unsigned int telemetryCadence)
{
  if (s_settings != NULL && s_settings->telemetryCadence != telemetryCadence)
  {
    s_settings->telemetryCadence = telemetryCadence;
    return SHARED_VALUE_CHANGED;
  }
  return SHARED_VALUE_UNCHANGED;
}
