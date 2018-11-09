#include "pch.h"
#include "UserServices.h"


using namespace scwin::services;

UserServices::UserServices()
{
}


UserServices::~UserServices()
{
}

task<RoleList^> scwin::services::UserServices::ListRolesAsync()
{
	return create_task([=]() -> RoleList^
	{
		auto list = ref new RoleList();
		list->Append(ref new UserRole("Admin"));
		list->Append(ref new UserRole("User"));
		return list;
	});
}
