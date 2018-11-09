#pragma once

#include <UserRole.h>
#include <UserProfile.h>

using namespace Platform;
using namespace Windows::Data::Json;

namespace scwin
{
	class AdminDataBuilder
	{
	public:
		static IVector<UserRole^>^ roles;

	public:
		static Vector<UserRole^>^ ConvertToUserRoles(JsonArray^ data);
		static UserProfileList^ ConvertToUsers(JsonArray^ data);
		static IAsyncOperation<IVector<UserRole^>^>^ GetUserRolesFromRoleIdsAsync(IVector<String^>^ roleIds);
	};
}

