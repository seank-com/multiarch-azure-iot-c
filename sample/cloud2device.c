#include "cloud2device.h"
#include "utils.h"

static IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE message, void* user_context)
{
  (void)user_context;

  IOTHUBMESSAGE_CONTENT_TYPE content_type = IoTHubMessage_GetContentType(message);
  if (content_type == IOTHUBMESSAGE_BYTEARRAY)
  {
    const unsigned char* buff_msg;
    size_t buff_len;

    (void)PrintMessageInformation(message);

    if (IoTHubMessage_GetByteArray(message, &buff_msg, &buff_len) != IOTHUB_MESSAGE_OK)
    {
      (void)printf("Failure retrieving byte array message\n");
    }
    else
    {
      (void)printf(" Binary message data: <<<%.*s>>> & Size=%d\n", (int)buff_len, buff_msg, (int)buff_len);
    }
  }
  else
  {
    const char* string_msg = IoTHubMessage_GetString(message);
    if (string_msg == NULL)
    {
      (void)printf("Failure retrieving byte array message\n");
    }
    else
    {
      (void)printf(" String message data: <<<%s>>>\n", string_msg);
    }
  }
  return IOTHUBMESSAGE_ACCEPTED;
}

void Cloud2Device_Init(IOTHUB_DEVICE_CLIENT_HANDLE deviceConnection)
{
  (void)IoTHubDeviceClient_SetMessageCallback(deviceConnection, receive_msg_callback, NULL); 
}