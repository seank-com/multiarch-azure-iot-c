#include "shared.h"
#include "devicetwin.h"

#include "azure_c_shared_utility/lock.h"

static IOTHUB_CLIENT_CONNECTION_STATUS s_status = IOTHUB_CLIENT_CONNECTION_AUTHENTICATED;
static TwinSettings* s_settings = NULL;
static LOCK_HANDLE s_lock = NULL;

void Shared_Init(char* softwareVersion, unsigned int defaultTelemetryCadence)
{
  s_lock = Lock_Init();
  if (s_lock == NULL)
  {
    (void)printf("ERROR: Failed to init lock");
  }
  s_settings = DeviceTwin_CreateTwinSettings(softwareVersion, defaultTelemetryCadence);
}

void Shared_Deinit()
{
  Lock_Deinit(s_lock);
  s_lock = NULL;
  DeviceTwin_DeleteTwinSettings(s_settings);
}

IOTHUB_CLIENT_CONNECTION_STATUS Shared_GetStatus()
{
  IOTHUB_CLIENT_CONNECTION_STATUS result = 0;
  if (s_lock != NULL)
  {
    Lock(s_lock);
    result = s_status;
    Unlock(s_lock);
  }
  return result;
}

int Shared_SetStatus(IOTHUB_CLIENT_CONNECTION_STATUS status)
{ 
  int result = SHARED_VALUE_UNCHANGED;
  if (s_lock != NULL)
  {
    Lock(s_lock);
    if (s_status != status)
    {
      s_status = status;
      result = SHARED_VALUE_CHANGED;
    }
    Unlock(s_lock);
  }
  return result;
}

const char* Shared_GetVersion()
{
  char* result = NULL;
  if (s_lock != NULL)
  {
    Lock(s_lock);
    if (s_settings != NULL)
    {
      result = s_settings->softwareVersion;
    }
    Unlock(s_lock);
  }
  return result;
}

unsigned int Shared_GetTelemetryCadence()
{
  unsigned int result = 0;
  if (s_lock != NULL)
  {
    Lock(s_lock);
    if (s_settings != NULL)
    {
      result = s_settings->telemetryCadence;
    }
    Unlock(s_lock);
  }
  return result;
}

int Shared_SetTelemetryCadence(unsigned int telemetryCadence)
{
  int result = SHARED_VALUE_UNCHANGED;
  if (s_lock != NULL)
  {
    Lock(s_lock);
    if (s_settings != NULL && s_settings->telemetryCadence != telemetryCadence)
    {
      s_settings->telemetryCadence = telemetryCadence;
      result = SHARED_VALUE_CHANGED;
    }
    Unlock(s_lock);
  }
  return result;
}
