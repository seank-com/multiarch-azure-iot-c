#include "device2cloud.h"

#include <stdlib.h>

#include "azure_c_shared_utility/uuid.h"

static char msgText[1024];

static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)userContextCallback;
    // When a message is sent this callback will get envoked
    (void)printf("Confirmation callback received for message with result %s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

void sendDevice2CloudMessage(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection)
{
  sprintf(msgText, "{\"foo\":\"bar\",\"random\":%d}", rand());

  IOTHUB_MESSAGE_HANDLE message_handle = IoTHubMessage_CreateFromString(msgText);

  UUID_T uuId = {};
  UUID_generate(&uuId);
  char* msgId = UUID_to_string(&uuId);
  (void)IoTHubMessage_SetMessageId(message_handle, msgId);

  (void)printf("\r\nSending message to IoTHub\r\nMessage: %s\r\n", msgText);
  IoTHubDeviceClient_SendEventAsync(deviceConnection, message_handle, send_confirm_callback, NULL);

  // The message is copied to the sdk so the we can destroy it
  IoTHubMessage_Destroy(message_handle);
}
