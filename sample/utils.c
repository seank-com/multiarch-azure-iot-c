#include "utils.h"

// Prints relevant system properties about a message.
void PrintMessageInformation(IOTHUB_MESSAGE_HANDLE message)
{
    const char* messageId;
    const char* correlationId;
    const char* inputQueueName;
    const char* connectionModuleId;
    const char* connectionDeviceId;

    // Message properties
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
    {
        messageId = "<null>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
    {
        correlationId = "<null>";
    }

    if ((inputQueueName = IoTHubMessage_GetInputName(message)) == NULL)
    {
        inputQueueName = "<null>";
    }

    if ((connectionModuleId = IoTHubMessage_GetConnectionModuleId(message)) == NULL)
    {
        connectionModuleId = "<null>";
    }

    if ((connectionDeviceId = IoTHubMessage_GetConnectionDeviceId(message)) == NULL)
    {
        connectionDeviceId = "<null>";
    }

    (void)printf("Received Message\n Message ID: [%s]\n Correlation ID: [%s]\n InputQueueName: [%s]\n connectionModuleId: [%s]\n connectionDeviceId: [%s]\n",
      messageId, correlationId, inputQueueName, connectionModuleId, connectionDeviceId);
}