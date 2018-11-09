#include "pch.h"
#include "ProductInventoryProperties.h"

DEFINERAISEPROPCHANGED(ProductInventoryProperties)


ProductInventoryProperties::ProductInventoryProperties()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
	invfilltype = ref new GeneralStatus(0, "Stockless", "Stockless");
}

scwin::ProductInventoryProperties::ProductInventoryProperties(String ^ id) : id(id)
{
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}
