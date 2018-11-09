#pragma once

#include "DtoBase.h"

namespace scwin
{
	public ref class scUserStatus sealed : public DtoBase
	{
	public:
		scUserStatus();

		scUserStatus(JsonObject^ data);

		virtual ~scUserStatus();

		property String^ Code
		{
			String^ get();
			void set(String^ value);
		}

		property String^ Desc
		{
			String^ get();
			void set(String^ value);
		}

	};
}
