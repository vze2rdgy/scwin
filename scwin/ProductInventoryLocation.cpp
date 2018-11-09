#include "pch.h"
#include "ProductInventoryLocation.h"

namespace scwin
{

	ProductInventoryLocation::ProductInventoryLocation()
	{
		this->id = SysInfoHelpers::NewCouchDbId();
		objState = DataState::Added;
		CanRaiseEvents = true;
	}

	ProductInventoryLocation::ProductInventoryLocation(String ^ id)
	{
		this->id = id;
		objState = DataState::Unmodified;
		CanRaiseEvents = true;
	}

	ProductInventoryLocation ^ ProductInventoryLocation::Clone()
	{
		ProductInventoryLocation ^ clone = ref new ProductInventoryLocation(id);
		clone->name = name;
		clone->code = code;
		clone->address = address;
		clone->objState = objState;
		return clone;
	}

	scwin::Address^ ProductInventoryLocation::Address::get()
	{
		return address;
	}

	void ProductInventoryLocation::Address::set(scwin::Address^ val)
	{
		if (address != val)
		{
			address = val;
			RaisePropertyChanged(PROPERTYNAME(Address));
		}
	}

	DataState ProductInventoryLocation::ObjectState::get()
	{
		return objState;
	}

	void ProductInventoryLocation::ObjectState::set(DataState val)
	{
		if (objState != val)
		{
			objState = val;
			RaisePropertyChanged(PROPERTYNAME(ObjectState));
		}
	}

	bool ProductInventoryLocation::IsValid::get()
	{
		if (StringHelpers::IsNullOrEmpty(name))
			return false;
		return true;
	}

	DEFINERAISEPROPCHANGED(ProductInventoryLocation)

}
