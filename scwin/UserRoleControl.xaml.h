//
// UserRoleControl.xaml.h
// Declaration of the UserRoleControl class
//

#pragma once

#include "UserRoleControl.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserRoleControl sealed
	{
	public:
		UserRoleControl();

		property RoleViewModel^ ViewModel;

	private:
		void btnRoleCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
