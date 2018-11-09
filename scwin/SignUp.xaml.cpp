//
// SignUp.xaml.cpp
// Implementation of the SignUp class
//

#include "pch.h"
#include "SignUp.xaml.h"

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

SignUp::SignUp(scMainViewModel^ vm)
{
	InitializeComponent();
	this->DataContext = vm->SignUpViewModel;
}

scSignupViewModel^ SignUp::ViewModel::get()
{
	return safe_cast<scSignupViewModel^>(this->DataContext);
}


void scwin::SignUp::PasswordBox_PasswordChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->Password = txtPassword->Password;
}


void scwin::SignUp::TextBox_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (StringHelpers::IsNullOrEmpty(ViewModel->Password))
	{
		ViewModel->Password = "";
	}
}


void scwin::SignUp::TextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}
