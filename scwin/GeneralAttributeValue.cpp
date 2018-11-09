#include "pch.h"
#include "GeneralAttributeValue.h"


GeneralAttributeValue::GeneralAttributeValue()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::GeneralAttributeValue::GeneralAttributeValue(String^ id)
{
	if (StringHelpers::IsNullOrEmpty(id))
		throw ref new InvalidArgumentException("id");
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

scwin::GeneralAttributeValue::~GeneralAttributeValue()
{
}

bool GeneralAttributeValue::IsValid::get()
{
	if (value == nullptr)
		return false;
	return true;
}

DEFINERAISEPROPCHANGED(GeneralAttributeValue)
