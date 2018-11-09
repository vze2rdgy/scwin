#include "pch.h"
#include "UserRole.h"

using namespace scwin;

UserRole::UserRole()
{
	Id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
}

scwin::UserRole::UserRole(String ^ Id)
{
	ObjectState = DataState::Unmodified;
	this->Id = Id;
}

UserRole ^ scwin::UserRole::Clone()
{
	UserRole^ r = ref new UserRole();
	r->Id = Id;
	r->name = name;
	r->rights = rights;
	r->sfuncRights = sfuncRights;
	r->ObjectState = ObjectState;
	return r;
}

void UserRole::RaisePropertyChanged(String ^ propName)
{
	if (ObjectState == DataState::Unmodified)
	{
		this->ObjectState = DataState::Edited;
	}
	WeakReference wr(this);
	AppDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([wr, propName]()
	{
		auto ths = wr.Resolve<UserRole>();
		if (ths)
		{
			ths->PropertyChanged(ths, ref new PropertyChangedEventArgs(propName));
		}
	}));
}

void scwin::UserRole::SetRightsAsString()
{
	String^ strRights;
	for (auto itr = FunctionalRightsDescriptions.begin(); 
		itr != FunctionalRightsDescriptions.end(); ++itr)
	{
		auto kvp = *itr;
		if (kvp.first == (kvp.first & rights))
		{
			strRights += StringHelpers::ConvertToString(kvp.second.Name.c_str());
			strRights += " | ";
		}
	}
	String^ output;
	if (StringHelpers::RemoveAtEnd(strRights, " | ", &output))
		FunctionalRightsAsString = output;
	else
		FunctionalRightsAsString = strRights;
}

uint64 scwin::UserRole::Rights::get()
{
	return rights;
}

void scwin::UserRole::Rights::set(uint64 val)
{
	if (rights != val)
	{
		rights = val;
		RaisePropertyChanged(PROPERTYNAME(Rights));
		SetRightsAsString();
	}
}

String ^ scwin::UserRole::FunctionalRightsAsString::get()
{
	return sfuncRights;
}

void scwin::UserRole::FunctionalRightsAsString::set(String^ val)
{
	if (sfuncRights != val)
	{
		sfuncRights = val;
		RaisePropertyChanged(PROPERTYNAME(FunctionalRightsAsString));
	}
}
