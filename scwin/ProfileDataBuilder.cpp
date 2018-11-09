#include "pch.h"
#include "ProfileDataBuilder.h"

using namespace Platform::Collections;
using namespace scwin;

#define name L"name"
#define roles L"roles"
#define email L"email"
#define password L"password"
#define company L"company"
#define fullname L"fullname"


UserContext ^ ProfileDataBuilder::Create(JsonObject ^ jo)
{
	//{"name":"charan@empire.com","roles":[{"role":"administrator","rights":["17"]}],"dbCode":"eqi(1.0.0.0)","fullname":"Charanjit S."}
	OutputDebugString(jo->ToString()->Data()); OutputDebugString(L"\n");
	auto uc = ref new UserContext();
	auto juc = jo;
	uc->Name = juc->GetNamedString(name, L"");
	uc->Key = juc->GetNamedString("dbCode", L"");
	
	JsonArray^ jroles;
	if ((jroles = juc->GetNamedArray(roles, nullptr)))
	{
		Vector<UserRole^>^ vroles = ref new Vector<UserRole^>(jroles->Size);
		for (auto roleandrights : jroles)
		{
			JsonObject^ roleObj = roleandrights->GetObject();
			// TODO: Recheck if the "role" is return UserRole's 'Id' property and not 'Name' property
			UserRole^ ur = ref new UserRole(roleObj->GetNamedString("role"));
			vroles->Append(ur);
			if (!roleObj->HasKey("rights"))
				uc->SetRight(FunctionalRights::NoRights);
			else 
			{
				uc->ClearRight();
				auto rights = roleObj->GetString();
				if (rights == "*")
					uc->SetRight(FunctionalRights::AllRights);
				else if (StringHelpers::IsNullOrEmpty(rights)) {}
				else
					uc->SetRight((FunctionalRights)std::strtoull(STRCONV(rights->Data()).c_str(), nullptr, 10));
			}
		}
		uc->Roles = vroles;
	}
	return uc;
}

JsonObject ^ scwin::ProfileDataBuilder::CreateSignupInfo(String ^ semail, String ^ spwd, String ^ scompany, String ^ sfullname)
{
	/*
	// msg sent
	{
	"email": "saj1@Walmart.com",
	"password": "password",
	"company": "WalmartA",
	"fullname": "Saji A"
	}

	// msg received when user exists

	{
	"code": 422,
	"result": "Userid already in use, please signin"
	}

	// success message.
	{
	"code": 200,
	"result": {
	"userCtx": {
	"name": "saj1@walmart.com",
	"roles": [],
	"dbCode": "wki"
	}
	}
	}
	*/

	JsonObject^ data = ref new JsonObject();
	data->SetNamedValue(email, JsonValue::CreateStringValue(semail));
	data->SetNamedValue(password, JsonValue::CreateStringValue(spwd));
	data->SetNamedValue(company, JsonValue::CreateStringValue(scompany));
	data->SetNamedValue(fullname, JsonValue::CreateStringValue(sfullname));
	return data;
}

JsonObject ^ scwin::ProfileDataBuilder::CreateSignInInfo(String ^ semail, String ^ spwd)
{
	/* request body

	{
		"email": "saj@Walmart.com",
			"password" : "password"
	}

	// response - ok
	{
	"code": 200,
	"result": {
	"userCtx": {
	"name": "saj@walmart.com",
	"roles": [],
	"dbCode": "lxd"
	}
	}
	}

	// not ok
	{
	"code": 401,
	"result": "Invalid userid or password, please try again [object Object]"
	}
	*/

	JsonObject^ data = ref new JsonObject();
	data->SetNamedValue(email, JsonValue::CreateStringValue(semail));
	data->SetNamedValue(password, JsonValue::CreateStringValue(spwd));
	return data;
}
