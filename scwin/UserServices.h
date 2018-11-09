#pragma once

#include "ObservableList.h"
#include "UserRole.h"

namespace scwin
{
	namespace services
	{

		typedef Vector<UserRole^> RoleList;

		class UserServices
		{
		public:
			UserServices();
			~UserServices();

			task<RoleList^> ListRolesAsync();

		};

	}
}
