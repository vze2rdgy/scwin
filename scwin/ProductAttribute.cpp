#include "pch.h"
#include "ProductAttribute.h"

namespace scwin
{

	ProductAttribute::ProductAttribute()
	{
		this->id = SysInfoHelpers::NewCouchDbId();
		ObjectState = DataState::Added;
		CanRaiseEvents = true;
	}

	ProductAttribute::ProductAttribute(String ^ id)
	{
		if (StringHelpers::IsNullOrEmpty(id))
			throw ref new InvalidArgumentException("id");

		this->id = id;
		ObjectState = DataState::Unmodified;
		CanRaiseEvents = true;
	}

	bool ProductAttribute::IsValid::get()
	{
		if (StringHelpers::IsNullOrEmpty(name) || StringHelpers::IsNullOrEmpty(type))
			return false;
		return true;
	}


	DEFINERAISEPROPCHANGED(ProductAttribute)
		
		
	// Note. the values returned here are based on 
	// the type returned by the function SysInfoHelpers::WinRtTypeToJsonType
	Type^ LookupAttributeValueType(String ^ attributeType)
	{
		if (attributeType == ATTR_CATEGORICAL)
		{
			return String::typeid;
		}
		else if (attributeType == ATTR_LABEL)
		{
			return String::typeid;
		}
		else if (attributeType == ATTR_METRIC)
		{
			return float::typeid;
		}
		else if (attributeType == ATTR_ORDINAL)
		{
			return UINT::typeid;
		}
		else if (attributeType == ATTR_PERIOD)
		{
			return DateTime::typeid;
		}
		else if (attributeType == ATTR_QUALITATIVE)
		{
			return String::typeid;
		}
		else
		{
			return String::typeid;
		}
	}
}