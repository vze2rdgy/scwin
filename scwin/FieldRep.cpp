#include "pch.h"
#include "FieldRep.h"


FieldRep::FieldRep()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::FieldRep::FieldRep(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}


FieldRep::~FieldRep()
{
}

IAsyncOperation<UserProfile^>^ FieldRep::User::get()
{
	if (!StringHelpers::IsNullOrEmpty(uid))
		throw ref new InvalidArgumentException("Invalid UserProfileId property");
	return LOCSVC->GetUser(uid);
}

IAsyncOperation<COrgCostCenter^>^ FieldRep::CostCenter::get()
{
	if (!StringHelpers::IsNullOrEmpty(corgccid))
		throw ref new InvalidArgumentException("Invalid CostCenterId property");
	return LOCSVC->GetCustomerCostCenterAsync(corgccid);
}

DEFINERAISEPROPCHANGED(FieldRep)