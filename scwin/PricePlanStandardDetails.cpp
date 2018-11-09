#include "pch.h"
#include "PricePlanStandardDetails.h"


PricePlanStandardDetails::PricePlanStandardDetails()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::PricePlanStandardDetails::PricePlanStandardDetails(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

bool PricePlanStandardDetails::IsValid::get()
{
	if (StringHelpers::IsNullOrEmpty(priceplanId))
		return false;
	if (price < 0.)
		return false;
	return true;
}


DEFINERAISEPROPCHANGED(PricePlanStandardDetails)
