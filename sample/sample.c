
#include <stdio.h>

#include "shared.h"
#include "device2cloud.h"
#include "cloud2device.h"
#include "devicetwin.h"
#include "directmethod.h"

#include "iothub.h"
#include "iothubtransportmqtt.h"
#include "azure_c_shared_utility/threadapi.h"


/* Paste in the your iothub device connection string  */
const char* connectionString = "HostName=seank.azure-devices.net;DeviceId=seank;SharedAccessKey=ywVFO6ZXrT2TubiOidT0ZC1Jf5JlAX0xaULNexJQ2bw=";

static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    (void)reason;
    (void)user_context;

    Shared_SetStatus(result);

    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        (void)printf("The device client is connected to IoT Edge\n");
    }
    else
    {
        (void)printf("The device client has been disconnected\n");
    }
}

static int connection_loop(void)
{
    int result = 0;
    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;

    protocol = MQTT_Protocol;

    (void)printf("Initializing IoTHub\n");
    if (IoTHub_Init() == 0)
    {
        (void)printf("Creating connection from connectionString\n");
        IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle = IoTHubDeviceClient_CreateFromConnectionString(connectionString, protocol);
        if (iotHubClientHandle != NULL)
        {
            // Uncomment the following lines to enable verbose logging (e.g., for debugging).
            //bool traceOn = true;
            //(void)IoTHubDeviceClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceOn);

            (void)printf("Registering connection_status_callback\n");
            (void)IoTHubDeviceClient_SetConnectionStatusCallback(iotHubClientHandle, connection_status_callback, NULL);

            (void)printf("Initializing Cloud2Device\n");
            (void)Cloud2Device_Init(iotHubClientHandle);

            (void)printf("Initializing DeviceTwin\n");
            (void)DeviceTwin_Init(iotHubClientHandle);

            (void)printf("Initializing DirectMethod\n");
            (void)DirectMethod_Init(iotHubClientHandle);


            while(Shared_GetStatus() == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
            {
                sendDevice2CloudMessage(iotHubClientHandle);
                ThreadAPI_Sleep(Shared_GetTelemetryCadence());
            }
            
            (void)printf("Initializing DeviceTwin\n");
            DeviceTwin_Deinit();

            (void)printf("Destroying connection\n");
            IoTHubDeviceClient_Destroy(iotHubClientHandle);
        }
        else
        {
            (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
        }

        (void)printf("Deinitializing IoTHub\n");
        IoTHub_Deinit();
    }
    else
    {
        printf("ERROR: Failed to initialize the platform.\r\n");
        result = 1;
    }
    return result;
}

int main(void)
{
    int result = 0;
    
    Shared_Init("1.0.0", 15 * 1000);
    while(result == 0)
    {
        printf("Starting connection_loop");
        result = connection_loop();
    }
    Shared_Deinit();
    return result;
}
