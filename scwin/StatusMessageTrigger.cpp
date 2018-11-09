#include "pch.h"
#include "StatusMessageTrigger.h"

DependencyProperty^ StatusMessageTrigger::messageTypeProperty = nullptr;


void scwin::StatusMessageTrigger::RegisterDps()
{

	messageTypeProperty = DependencyProperty::Register("MessageType",
		StatusMessageTypes::typeid, StatusMessageTrigger::typeid,
		ref new PropertyMetadata(StatusMessageTypes::Information));

}

StatusMessageTrigger::StatusMessageTrigger()
{
}


StatusMessageTrigger::~StatusMessageTrigger()
{
}

