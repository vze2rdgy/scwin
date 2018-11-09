#include "pch.h"
#include "CustomerContact.h"


CustomerContact::CustomerContact()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::CustomerContact::CustomerContact(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}


CustomerContact::~CustomerContact()
{

}

DEFINERAISEPROPCHANGED(CustomerContact)