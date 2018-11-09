#include "pch.h"
#include "PricePlanTierDetails.h"


PricePlanTierDetails::PricePlanTierDetails()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::PricePlanTierDetails::PricePlanTierDetails(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

DEFINERAISEPROPCHANGED(PricePlanTierDetails)
