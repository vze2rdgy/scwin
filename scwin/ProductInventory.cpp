#include "pch.h"
#include "ProductInventory.h"

namespace scwin
{

	ProductInventory::ProductInventory()
	{
		CanRaiseEvents = true;
	}

	DEFINERAISEPROPCHANGED(ProductInventory)

}
