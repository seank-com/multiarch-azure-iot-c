#include "directmethod.h"
#include "shared.h"

#include <stdlib.h> // malloc

static int deviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
    (void)payload;
    (void)size;
    (void)userContextCallback;

    int result;

    if (strcmp("resetConnection", method_name) == 0)
    {
        const char deviceMethodResponse[] = "{ \"Response\": \"Aye aye captain\" }";
        *response_size = sizeof(deviceMethodResponse)-1;
        *response = malloc(*response_size);
        (void)memcpy(*response, deviceMethodResponse, *response_size);

        Shared_SetStatus(IOTHUB_CLIENT_CONNECTION_UNAUTHENTICATED);
        
        result = 200;
    }
    else
    {
        // All other entries are ignored.
        const char deviceMethodResponse[] = "{ }";
        *response_size = sizeof(deviceMethodResponse)-1;
        *response = malloc(*response_size);
        (void)memcpy(*response, deviceMethodResponse, *response_size);
        result = -1;
    }

    return result;
}

void DirectMethod_Init(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection)
{
  (void)IoTHubDeviceClient_SetDeviceMethodCallback(deviceConnection, deviceMethodCallback, NULL);
}


