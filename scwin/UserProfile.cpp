#include "pch.h"
#include "UserProfile.h"
#include "AdminDataBuilder.h"

using namespace scwin;


void scwin::UserProfile::UpdateRoleStr()
{
	if (roles != nullptr)
	{
		create_task(AdminDataBuilder::GetUserRolesFromRoleIdsAsync(this->roles))
			.then([this](IVector<UserRole^>^ userRoles)
		{
			String^ output;
			if (userRoles != nullptr)
			{
				for (auto r : userRoles)
				{
					output += r->Name;
					output += " | ";
				}
				StringHelpers::RemoveAtEnd(output, " | ", &output);
			}
			MemberRolesAsString = output;
		});
	}
}

UserProfile::UserProfile()
{
	ObjectState = DataState::Added;
}

scwin::UserProfile::UserProfile(
	String ^ email, 
	String ^ fullName, 
	String ^ dbKey, 
	IObservableVector<String^>^ roles)
{
	ObjectState = DataState::Unmodified;
	this->id = email;
	this->fullName = fullName;
	this->dbKey = dbKey;
	this->roles = roles;
	UpdateRoleStr();
}

String^ UserProfile::MemberRolesAsString::get()
{
	return memroleAsString;
}

void UserProfile::MemberRolesAsString::set(String^ val)
{
	if (memroleAsString != val)
	{
		memroleAsString = val;
		RaisePropertyChanged(PROPERTYNAME(MemberRolesAsString));
	}
}


UserProfile ^ scwin::UserProfile::Clone()
{
	UserProfile^ prof = ref new UserProfile();
	prof->id = id;
	prof->fullName = fullName;
	prof->dbKey = dbKey;
	prof->scode = scode;
	prof->roles = ref new Vector<String^>(to_vector(roles));
	prof->datastate = datastate;
	return prof;
}

bool UserProfile::IsValid::get()
{
	return !scwin::utils::StringHelpers::IsNullOrEmpty(id) 
		&&
		!scwin::utils::StringHelpers::IsNullOrEmpty(fullName) 
		&& 
		roles != nullptr 
		&& 
		roles->Size;
}

DEFINERAISEPROPCHANGED(UserProfile)