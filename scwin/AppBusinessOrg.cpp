#include "pch.h"
#include "AppBusinessOrg.h"


AppBusinessOrg::AppBusinessOrg()
{
	this->Id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;

}

scwin::AppBusinessOrg::AppBusinessOrg(String ^ id)
{
	this->Id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

String^ AppBusinessOrg::Name::get()
{
	return _name;
}

void AppBusinessOrg::Name::set(String^ val)
{
	if (val != _name)
	{
		_name = val;
		RaisePropertyChanged(PROPERTYNAME(Name));
	}
}

String^ AppBusinessOrg::AddressId::get()
{
	return _addressId;
}

void AppBusinessOrg::AddressId::set(String^ val)
{
	if (_addressId != val)
	{
		_addressId = val;
		RaisePropertyChanged(PROPERTYNAME(AddressId));
	}
}

scwin::Address^ AppBusinessOrg::Address::get()
{
	if (_addressId == nullptr)
		return nullptr;
	return scLocalDataService::Instance()->GetAddress(_addressId);
}

String^ AppBusinessOrg::LogoImageId::get()
{
	return _logoImageId;
}

void AppBusinessOrg::LogoImageId::set(String^ val)
{
	if (_logoImageId != val)
	{
		_logoImageId = val;
		RaisePropertyChanged(PROPERTYNAME(LogoImageId));
	}
}

TechContact^ AppBusinessOrg::TechContact::get()
{
	return _techContact;
}

void AppBusinessOrg::TechContact::set(scwin::TechContact^ val)
{
	if (val == nullptr)
	{
		_techContact = nullptr;
		RaisePropertyChanged(PROPERTYNAME(TechContact));
		return;
	}
	if (_techContact == nullptr ||
		!_techContact->Equals(val))
	{
		_techContact = val;
		RaisePropertyChanged(PROPERTYNAME(TechContact));
	}
}

String^ AppBusinessOrg::Website::get()
{
	return _website;
}

void AppBusinessOrg::Website::set(String^ val)
{
	if (_website != val)
	{
		_website = val;
		RaisePropertyChanged(PROPERTYNAME(Website));
	}
}

scwin::HelpDesk^ AppBusinessOrg::HelpDesk::get()
{
	return _helpdesk;
}

void AppBusinessOrg::HelpDesk::set(scwin::HelpDesk^ val)
{
	if (val == nullptr)
	{
		_helpdesk = nullptr;
		RaisePropertyChanged(PROPERTYNAME(HelpDesk));
		return;
	}
	if (_helpdesk == nullptr ||
		!_helpdesk->Equals(val))
	{
		_helpdesk = val;
		RaisePropertyChanged(PROPERTYNAME(HelpDesk));
	}
}

bool AppBusinessOrg::IsEmpty::get()
{
	return StringHelpers::IsNullOrEmpty(Name);
}

bool AppBusinessOrg::IsValid::get()
{
	return !StringHelpers::IsNullOrEmpty(Name);
}

DEFINERAISEPROPCHANGED(AppBusinessOrg);

bool scwin::TechContact::Equals(TechContact ^ obj)
{
	if (obj == nullptr)
		return false;
	return obj->Name == Name &&
		obj->Phone == Phone &&
		obj->Email == Email;
}

bool scwin::HelpDesk::Equals(HelpDesk ^ obj)
{
	if (obj == nullptr)
		return false;
	return Phone == obj->Phone && 
		Email != obj->Email &&
		Url != obj->Url;
}
