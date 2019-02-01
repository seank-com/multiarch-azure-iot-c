#include "devicetwin.h"
#include "shared.h"

#include <stdio.h>
#include <stdlib.h>

#include "parson.h"

#define CONFIG_FILE "./config.json"

static IOTHUB_DEVICE_CLIENT_HANDLE s_deviceConnection = NULL;

TwinSettings* DeviceTwin_CreateTwinSettings(char* softwareVersion, unsigned int telemetryCadence)
{
  TwinSettings* settings = malloc(sizeof(TwinSettings));
  if (settings == NULL)
  {
    (void)printf("ERROR: Failed to allocate memory\n");
    return NULL;
  }

  (void)memset(settings,0,sizeof(TwinSettings));
  if (softwareVersion != NULL)
  {
    settings->softwareVersion =  malloc(strlen(softwareVersion)+1);
    if (settings->softwareVersion != NULL)
    {
      (void)strcpy(settings->softwareVersion, softwareVersion);
    }
  }

  if (telemetryCadence != 0)
  {
    settings->telemetryCadence = telemetryCadence;
  }

  return settings;
}

void DeviceTwin_DeleteTwinSettings(TwinSettings* settings)
{
  if (settings != NULL)
  {
    if (settings->softwareVersion != NULL)
    {
      free(settings->softwareVersion);
    }
    free(settings);
  }
}

static JSON_Value* createJSONValue()
{
  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  // Only reported properties:
  (void)json_object_set_string(root_object, "softwareVersion", Shared_GetVersion());
  (void)json_object_set_number(root_object, "telemetryCadence", Shared_GetTelemetryCadence());

  return root_value;
}

static char* serializeToJson()
{
  JSON_Value* root_value = createJSONValue();
  char* result = json_serialize_to_string(root_value);

  json_value_free(root_value);
  return result;
}

static void parseFromValue(JSON_Value* root_value, DEVICE_TWIN_UPDATE_STATE update_state, TwinSettings* settings)
{
  if (root_value != NULL && settings != NULL)
  {
    JSON_Object* root_object = json_value_get_object(root_value);

    // Only desired properties:
    JSON_Value* telemetryCadence;
    if (update_state == DEVICE_TWIN_UPDATE_COMPLETE)
    {
      telemetryCadence = json_object_dotget_value(root_object, "desired.telemetryCadence");
    }
    else
    {
      telemetryCadence = json_object_get_value(root_object, "telemetryCadence");
    }

    if (telemetryCadence != NULL)
    {
      settings->telemetryCadence = (uint32_t)json_value_get_number(telemetryCadence);
    }
  }
}

static TwinSettings* parseFromString(const char* json, DEVICE_TWIN_UPDATE_STATE update_state)
{
  JSON_Value* root_value = json_parse_string(json);
  TwinSettings* settings = DeviceTwin_CreateTwinSettings(NULL, 0);
  
  parseFromValue(root_value, update_state, settings);
  
  json_value_free(root_value);
  return settings;
}

static TwinSettings* parseFromFile(const char* filename)
{
  JSON_Value* root_value = json_parse_file(filename);
  TwinSettings* settings = DeviceTwin_CreateTwinSettings(NULL, 0);
  
  parseFromValue(root_value, DEVICE_TWIN_UPDATE_PARTIAL, settings);
  
  json_value_free(root_value);
  return settings;
}

static void loadConfig()
{
  TwinSettings* settings = parseFromFile(CONFIG_FILE);
  if (settings != NULL)
  {
    if (settings->telemetryCadence != 0)
    {
      Shared_SetTelemetryCadence(settings->telemetryCadence);
    }
    DeviceTwin_DeleteTwinSettings(settings);
  }
}

static void saveConfig()
{
  JSON_Value* root_value = createJSONValue();
  json_serialize_to_file(root_value, CONFIG_FILE);
  json_value_free(root_value);
}

static void reportedStateCallback(int status_code, void* userContextCallback)
{
    (void)userContextCallback;
    printf("Device Twin reported properties update completed with result: %d\r\n", status_code);
}

static void updateReportedState()
{
  if (s_deviceConnection != NULL)
  {
    char* buffer = serializeToJson();
    if (buffer)
    {
      (void)IoTHubDeviceClient_SendReportedState(s_deviceConnection, (const unsigned char*)buffer, strlen(buffer), reportedStateCallback, NULL);
      printf("Send Reported State: %s\n", buffer);
      free(buffer);
    }
  }
}

static void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
  (void)size;
  (void)userContextCallback;

  TwinSettings* changes = parseFromString((const char*)payLoad, update_state);
  if(changes != NULL && changes->telemetryCadence != 0)
  {
    if (Shared_SetTelemetryCadence(changes->telemetryCadence) == SHARED_VALUE_CHANGED)
    {
      saveConfig();
      updateReportedState();
    }
  }
}

void DeviceTwin_Init(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection)
{
  s_deviceConnection = deviceConnection;
  loadConfig();
  updateReportedState();
  saveConfig();
  (void)IoTHubDeviceClient_SetDeviceTwinCallback(deviceConnection, deviceTwinCallback, NULL);
}

void DeviceTwin_Deinit()
{
  s_deviceConnection = NULL;
}
