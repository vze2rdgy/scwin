//
// SignIn.xaml.cpp
// Implementation of the SignIn class
//

#include "pch.h"
#include "SignIn.xaml.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

SignIn::SignIn(scMainViewModel^ vm)
{
	InitializeComponent();
	this->DataContext = vm->SignInViewModel;
	this->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::SignIn::OnLoaded);
}


void scwin::SignIn::btnSignIn_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}


void scwin::SignIn::PasswordBox_PasswordChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	safe_cast<scSignInViewModel^>(this->DataContext)->Password = this->txtPassword->Password;
}


void scwin::SignIn::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto fm = dynamic_cast<Control^>(FocusManager::FindFirstFocusableElement(this));
	if (fm)
	{
		fm->Focus(Windows::UI::Xaml::FocusState::Programmatic);
	}
}
