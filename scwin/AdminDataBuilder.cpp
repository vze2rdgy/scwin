#include "pch.h"
#include "AdminDataBuilder.h"

using namespace scwin;

IVector<UserRole^>^ AdminDataBuilder::roles = nullptr;

Vector<UserRole^>^ scwin::AdminDataBuilder::ConvertToUserRoles(JsonArray ^ data)
{
	auto col = ref new Vector<UserRole^>();
	
	if (data == nullptr)
		return col;

	OutputDebugString(data->ToString()->Data()); OutputDebugString(L"\n");
	/*
	{
	  "code": 200,
	  "result": [
		"HELPER",
		"ADMINISTRATOR"
	  ]
	}
	*/

	if (data->ValueType != JsonValueType::Array)
		return col;

	//[{"role":"administrator","functionalrights":"*"},{"role":"fieldrep","functionalrights":"17"},{"role":"reportgen","functionalrights":"21"}]

	for (IJsonValue^ rn : data)
	{
		JsonObject^ rnobj = rn->GetObject();
		auto role = ref new UserRole();
		role->Id = rnobj->GetNamedString("id");
		if (StringHelpers::IsNullOrEmpty(role->Id))
			throw Exception::CreateException(E_FAIL, "Role object has no 'Id' property defined.");
		role->Name = rnobj->GetNamedString("role", "");
		String^ sRight = rnobj->GetNamedString("functionalrights", "");
		if (StringHelpers::IsNullOrEmpty(sRight))
			role->Rights = FunctionalRights::NoRights;
		else if (sRight == "*")
			role->Rights = FunctionalRights::AllRights;
		else
			role->Rights = std::strtoull(STRCONV(sRight).c_str(), nullptr, 10);
		col->Append(role);
	}
	return col;
}



UserProfileList^ scwin::AdminDataBuilder::ConvertToUsers(JsonArray^ data)
{
	/*
	{
	"code": 200,
	"result": [
	{
	"id": "org.couchdb.user:charan@empire.com",
	"key": "eqi",
	"value": {
	"fullname": "Charanjit S.",
	"roles": [
	"administrator"
	]
	}
	},
	{
	"id": "org.couchdb.user:ravi@empire.com",
	"key": "eqi",
	"value": {
	"fullname": "rocket singh",
	"roles": [
	"salesrep"
	]
	}
	}
	]
	}	*/
	auto col = ref new UserProfileList();
	for (IJsonValue^ rn : data)
	{
		JsonObject^ val = rn->GetObject();
		JsonObject^ usr = val->GetNamedObject(L"value");
		String^ email = val->GetNamedString(L"id");
		email = StringHelpers::Replace(email, "org.couchdb.user:", "");
		String^ fullName = usr->GetNamedString(L"fullname");
		String^ dbKey = val->GetNamedString(L"key");
		// populate roles
		JsonArray^ rls = usr->GetNamedArray(L"roles");
		Vector<String^>^ roles = ref new Vector<String ^>();
		for (IJsonValue^ r : rls)
		{
			roles->Append(r->GetString());
		}
		col->Append(ref new UserProfile(email, fullName, dbKey, roles));
	}
	return col;

}

IAsyncOperation<IVector<UserRole^>^>^ scwin::AdminDataBuilder::GetUserRolesFromRoleIdsAsync(IVector<String^>^ roleIds)
{
	return create_async([=]() -> IVector<UserRole^>^
	{
		auto lvs = scLocalDataService::Instance();
		if (roles == nullptr)
			roles = create_task(lvs->ListRoles()).get();
		auto listOfRoles = to_vector((Vector<UserRole^>^)(roles));
		Vector<UserRole^>^ roles = ref new Vector<UserRole^>();
		for (auto sid : roleIds)
		{
			auto itrole = std::find_if(listOfRoles.begin(), listOfRoles.end(), [sid](UserRole^ r)
			{
				return r->Id == sid;
			});
			if (itrole != listOfRoles.end())
				roles->Append(*itrole);
		}
		return roles;
	});
}
