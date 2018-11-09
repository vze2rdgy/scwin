//
// UserProfileEditView.xaml.h
// Declaration of the UserProfileEditView class
//

#pragma once

#include "UserProfileEdit.g.h"
#include "UserProfile.h"
#include "AdminViewModel.h"


namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserProfileEdit sealed
	{
	public:
		UserProfileEdit();
	private:
		void HyperlinkButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void PasswordBox_PasswordChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Flyout_Opening(Platform::Object^ sender, Platform::Object^ e);
		void Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		property UserViewModel^ ViewModel
		{
			UserViewModel^ get()
			{
				return safe_cast<UserViewModel^>(this->DataContext);
			}
		}

		void btnClose_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void pwdFirst_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
