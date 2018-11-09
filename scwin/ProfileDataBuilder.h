#pragma once

#include "UserContext.h"

using namespace Platform;
using namespace Windows::Data::Json;

namespace scwin
{
	ref class ProfileDataBuilder sealed
	{
	public:
		static UserContext^ Create(JsonObject^ jo);
		static JsonObject^ CreateSignupInfo(String^ semail, String^ spwd, String^ scompany, String^ sfullname);
		static JsonObject^ CreateSignInInfo(String^ semail, String^ pwd);
	};
}