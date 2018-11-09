#include "pch.h"
#include "StatusMessage.h"


StatusMessage::StatusMessage(String^ message, StatusMessageTypes type)
{
	Message = message;
	Type = type;
}
