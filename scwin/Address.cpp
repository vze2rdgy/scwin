#include "pch.h"
#include "Address.h"


Address::Address()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::Address::Address(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}


Address::~Address()
{
}

String ^ scwin::Address::ToString()
{
	std::wstringstream ostr;
	if (!StringHelpers::IsNullOrEmpty(line1))
		ostr << line1->Data();
	if (!StringHelpers::IsNullOrEmpty(line2))
	{
		ostr << L", ";
		ostr << line2->Data();
	}
	if (!StringHelpers::IsNullOrEmpty(city))
	{
		ostr << L", ";
		ostr << city->Data();
	}
	if (!StringHelpers::IsNullOrEmpty(province))
	{
		ostr << L", ";
		ostr << province->Data();
	}
	if (!StringHelpers::IsNullOrEmpty(country))
	{
		ostr << L", ";
		ostr << country->Data();
	}
	return ref new String(ostr.str().c_str());
}

bool Address::IsValid::get()
{
	if (
		StringHelpers::IsNullOrEmpty(line1) ||
		StringHelpers::IsNullOrEmpty(city) ||
		StringHelpers::IsNullOrEmpty(province) ||
		StringHelpers::IsNullOrEmpty(country)
		)
		return false;
	return true;
}

DEFINERAISEPROPCHANGED(Address)