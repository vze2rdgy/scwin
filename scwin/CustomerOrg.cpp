#include "pch.h"
#include "CustomerOrg.h"


CustomerOrg::CustomerOrg()
{
	this->id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

CustomerOrg::CustomerOrg(String^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}


CustomerOrg::~CustomerOrg()
{
}

DEFINERAISEPROPCHANGED(CustomerOrg)