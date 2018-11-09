#include "pch.h"
#include "ProductPricePlan.h"

namespace scwin
{


	ProductPricePlan::ProductPricePlan()
	{
		ObjectState = DataState::Added;
		id = SysInfoHelpers::NewCouchDbId();
		CanRaiseEvents = true;
	}

	ProductPricePlan::ProductPricePlan(String^ id)
	{
		ObjectState = DataState::Unmodified;
		this->id = id;
		CanRaiseEvents = true;
	}

	bool ProductPricePlan::IsEmpty::get()
	{
		if (StringHelpers::IsNullOrEmpty(code))
			return true;
		if (dateInit.UniversalTime == 0)
			return true;
		return false;
	}

	bool ProductPricePlan::IsValid::get()
	{
		if (StringHelpers::IsNullOrEmpty(code))
			return false;
		if (dateInit.UniversalTime == 0)
			return false;
		return true;
	}

	DEFINERAISEPROPCHANGED(ProductPricePlan)
}