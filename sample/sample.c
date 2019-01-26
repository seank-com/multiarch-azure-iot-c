// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This sample shows how to translate the Device Twin json received from Azure IoT Hub into meaningful data for your application.
// It uses the parson library, a very lightweight json parser.

// There is an analogous sample using the serializer - which is a library provided by this SDK to help parse json - in devicetwin_simplesample.
// Most applications should use this sample, not the serializer.

// WARNING: Check the return of all API calls when developing your solution. Return checks ommited for sample simplification.

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"

/* Paste in the your iothub device connection string  */
static const char* connectionString = "[connection string]";

static char msgText[1024];

static IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE message, void* user_context)
{
    (void)user_context;
    const char* messageId;
    const char* correlationId;

    // Message properties
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
    {
        messageId = "<unavailable>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
    {
        correlationId = "<unavailable>";
    }

    IOTHUBMESSAGE_CONTENT_TYPE content_type = IoTHubMessage_GetContentType(message);
    if (content_type == IOTHUBMESSAGE_BYTEARRAY)
    {
        const unsigned char* buff_msg;
        size_t buff_len;

        if (IoTHubMessage_GetByteArray(message, &buff_msg, &buff_len) != IOTHUB_MESSAGE_OK)
        {
            (void)printf("Failure retrieving byte array message\r\n");
        }
        else
        {
            (void)printf("Received Binary message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n", messageId, correlationId, (int)buff_len, buff_msg, (int)buff_len);
        }
    }
    else
    {
        const char* string_msg = IoTHubMessage_GetString(message);
        if (string_msg == NULL)
        {
            (void)printf("Failure retrieving byte array message\r\n");
        }
        else
        {
            (void)printf("Received String Message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%s>>>\r\n", messageId, correlationId, string_msg);
        }
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

static int deviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
    (void)userContextCallback;
    (void)payload;
    (void)size;

    int result;

    if (strcmp("getCarVIN", method_name) == 0)
    {
        const char deviceMethodResponse[] = "{ \"Response\": \"1HGCM82633A004352\" }";
        *response_size = sizeof(deviceMethodResponse)-1;
        *response = malloc(*response_size);
        (void)memcpy(*response, deviceMethodResponse, *response_size);
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

static void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    (void)update_state;
    (void)payLoad;
    (void)size;
    (void)userContextCallback;
}

static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)userContextCallback;
    // When a message is sent this callback will get envoked
    (void)printf("Confirmation callback received for message with result %s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

static void reportedStateCallback(int status_code, void* userContextCallback)
{
    (void)userContextCallback;
    printf("Device Twin reported properties update completed with result: %d\r\n", status_code);
}

static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    (void)reason;
    (void)user_context;
    // This sample DOES NOT take into consideration network outages.
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        (void)printf("The device client is connected to IoT Edge\r\n");
    }
    else
    {
        (void)printf("The device client has been disconnected\r\n");
    }
}

int main(void)
{
    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    IOTHUB_DEVICE_CLIENT_HANDLE iotHubClientHandle;
    IOTHUB_MESSAGE_HANDLE message_handle;

    protocol = MQTT_Protocol;

    if (IoTHub_Init() != 0)
    {
        (void)printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        if ((iotHubClientHandle = IoTHubDeviceClient_CreateFromConnectionString(connectionString, protocol)) == NULL)
        {
            (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
        }
        else
        {
            // Uncomment the following lines to enable verbose logging (e.g., for debugging).
            //bool traceOn = true;
            //(void)IoTHubDeviceClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceOn);

            (void)IoTHubDeviceClient_SetMessageCallback(iotHubClientHandle, receive_msg_callback, NULL);
            (void)IoTHubDeviceClient_SetConnectionStatusCallback(iotHubClientHandle, connection_status_callback, NULL);

            sprintf(msgText, "{\"firmwareVersion\": 3.0 }");

            (void)IoTHubDeviceClient_SendReportedState(iotHubClientHandle, (const unsigned char*)msgText, strlen(msgText), reportedStateCallback, NULL);
            (void)IoTHubDeviceClient_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
            (void)IoTHubDeviceClient_SetDeviceTwinCallback(iotHubClientHandle, deviceTwinCallback, NULL);

            for(int i = 0;i < 5; i += 1)
            {
                sprintf(msgText, "{\"foo\":\"bar\",\"random\":%d}", rand());

                message_handle = IoTHubMessage_CreateFromString(msgText);

                // Set Message property
                //(void)IoTHubMessage_SetMessageId(message_handle, "MSG_ID");
                //(void)IoTHubMessage_SetCorrelationId(message_handle, "CORE_ID");
                //(void)IoTHubMessage_SetContentTypeSystemProperty(message_handle, "application%2fjson");
                //(void)IoTHubMessage_SetContentEncodingSystemProperty(message_handle, "utf-8");

                // Add custom properties to message
                //(void)IoTHubMessage_SetProperty(message_handle, "property_key", "property_value");

                (void)printf("\r\nSending message to IoTHub\r\nMessage: %s\r\n", msgText);
                IoTHubDeviceClient_SendEventAsync(iotHubClientHandle, message_handle, send_confirm_callback, NULL);

                // The message is copied to the sdk so the we can destroy it
                IoTHubMessage_Destroy(message_handle);
                ThreadAPI_Sleep(5000);
            }

            IoTHubDeviceClient_Destroy(iotHubClientHandle);
        }

        IoTHub_Deinit();
    }
}
