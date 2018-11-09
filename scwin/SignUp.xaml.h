//
// SignUp.xaml.h
// Declaration of the SignUp class
//

#pragma once

#include "SignUp.g.h"
#include "scMainViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SignUp sealed
	{
	public:
		SignUp(scMainViewModel^ vm);
	private:
		property scSignupViewModel^ ViewModel
		{
			scSignupViewModel^ get();
		}
		void PasswordBox_PasswordChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextBox_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
