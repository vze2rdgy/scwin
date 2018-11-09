//
// SignIn.xaml.h
// Declaration of the SignIn class
//

#pragma once

#include "scMainViewModel.h"
#include "SignIn.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SignIn sealed
	{
	public:
		SignIn(scMainViewModel^ vm);
	private:
		void btnSignIn_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void PasswordBox_PasswordChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
