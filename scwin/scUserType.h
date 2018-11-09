#pragma once

#include "DtoBase.h"

namespace scwin 
{

	public ref class scUserType sealed : DtoBase
	{
	public:
		scUserType();
		scUserType(JsonObject^ data);
		String^ Name();
		void setName(String^ name);
	};

}