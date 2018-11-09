#pragma once
#include "DtoBase.h"
namespace scwin
{
	public ref class scUser sealed : DtoBase
	{
	public:
		scUser();
		virtual ~scUser();

		/////Email address. This is the login of the user.
		//property String^ EmailAddress
		//{
		//	String^ get();
		//	void set(String^ value);
		//}

		///// User Role Id
		//property String^ RoleId
		//{
		//	String^ get();
		//	void set(String^ value);
		//}


	};

}