#include "devicetwin.h"

//#include <stdio.h>
#include <stdlib.h> // malloc
#include <unistd.h> // access
// #include <string.h>

#include "parson.h"

#define CONFIG_FILE "./config.json"

TwinSettings g_settings = {0};

static IOTHUB_DEVICE_CLIENT_HANDLE s_deviceConnection = NULL;

static char* serializeToJson()
{
  char* result;

  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  // Only reported properties:
  (void)json_object_set_string(root_object, "softwareVersion", g_settings.softwareVersion);
  (void)json_object_set_number(root_object, "telemetryCadence", g_settings.telemetryCadence);

  result = json_serialize_to_string(root_value);

  json_value_free(root_value);

  return result;
}

static void parseFromJson(const char* json, DEVICE_TWIN_UPDATE_STATE update_state, TwinSettings* pSettings)
{
  JSON_Value* root_value = NULL;
  JSON_Object* root_object = NULL;

  root_value = json_parse_string(json);
  root_object = json_value_get_object(root_value);

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
    pSettings->telemetryCadence = (uint32_t)json_value_get_number(telemetryCadence);
  }

  json_value_free(root_value);
}

static void loadConfig()
{
  int result = access(CONFIG_FILE, F_OK);
  if (result == 0)
  {
    FILE* f = fopen(CONFIG_FILE, "r");
    if (f != NULL)
    {
        (void)fseek(f, 0, SEEK_END);
        size_t fs = ftell(f) + 1;
        (void)fseek(f, 0, SEEK_SET);
        if (fs > 0)
        {
          char* buffer = malloc(fs);
          if (buffer != NULL)
          {
            size_t rs = fread(buffer, 1, fs - 1, f);
            if (rs == (fs - 1))
            {
              (void)parseFromJson(buffer, DEVICE_TWIN_UPDATE_PARTIAL, &g_settings);
            }
            else
            {
              printf("Error: failed to read config file\n");
            }
            free(buffer);
          }
          else
          {
            printf("Error: failed to allocate memory\n");
          }
        }
        else
        {
          printf("Error: config file empty\n");
        }
        fclose(f);
    }
    else
    {
      printf("Error: failed to open config file\n");
    }
  }
  else
  {
    printf("Warn: config file does not exist\n");
  }
}

static void saveConfig()
{
  char* buffer = serializeToJson();
  if (buffer)
  {
    FILE* f = fopen(CONFIG_FILE, "w+");
    if (f != NULL)
    {
      size_t len =  strlen(buffer);
      size_t ws = fwrite(buffer, 1, len, f);
      if (ws != len)
      {
        printf("Error: Cannot write file\n");
      }
      fclose(f);
    }
    else
    {
      printf("Error: Could not open file for writing\n");
    }
    free(buffer);
  }
  else
  {
    printf("Error: failed to serial object\n");
  }
  
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

  TwinSettings changes = {0};
  
  (void)parseFromJson((const char*)payLoad, update_state, &changes);

  if (changes.telemetryCadence != 0)
  {
    printf("Received a new telemetryCadence = %d\n", changes.telemetryCadence);
    g_settings.telemetryCadence = changes.telemetryCadence;

    (void)saveConfig();
    (void)updateReportedState();
  }
}

void DeviceTwin_Init(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection)
{
  s_deviceConnection = deviceConnection;
  loadConfig();
  (void)updateReportedState();
  (void)IoTHubDeviceClient_SetDeviceTwinCallback(deviceConnection, deviceTwinCallback, NULL);
}

void DeviceTwin_Deinit()
{
  s_deviceConnection = NULL;
}
