//
// UserProfileEditView.xaml.cpp
// Implementation of the UserProfileEditView class
//

#include "pch.h"
#include "UserProfileEdit.xaml.h"

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

UserProfileEdit::UserProfileEdit()
{
	InitializeComponent();
}


void scwin::UserProfileEdit::HyperlinkButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	FlyoutBase::ShowAttachedFlyout((FrameworkElement^)sender);
}


void scwin::UserProfileEdit::PasswordBox_PasswordChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void scwin::UserProfileEdit::Flyout_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	auto flyOut = safe_cast<Flyout^>(sender);
	Windows::UI::Xaml::Style^ style = ref new Windows::UI::Xaml::Style();
	style->TargetType = FlyoutPresenter::typeid;
	Windows::UI::Xaml::Setter^ setter = ref new Windows::UI::Xaml::Setter(gridEntryLayout->MinWidthProperty, gridEntryLayout->ActualWidth);
	style->Setters->Append(setter);
	flyOut->FlyoutPresenterStyle = style;
}


void scwin::UserProfileEdit::Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	if (pwdFirst->Password != pwdConfirm->Password || !StringHelpers::IsValidPassword(pwdFirst->Password))
	{
		auto ret = VisualStateManager::GoToState(SetupPasswordFlyout, "PasswordDontMatchOrInvalidpassword", false);
		OutputDebugString(ret ? L"Yes": L"No");

		if (pwdFirst->Password != pwdConfirm->Password)
		{
			tbMessage->Text = ref new String(GetMessage(MessageIds::PasswordsDoNotMatch));
		}
		else
		{
			tbMessage->Text = ref new String(GetMessage(MessageIds::PasswordNotValid));
		}
		return;
	}
	ViewModel->UserPwd = pwdFirst->Password;
	auto ret = VisualStateManager::GoToState(SetupPasswordFlyout, "AllGood", false);
	OutputDebugString(ret ? L"Yes" : L"No");
	PasswordSetupFlyout->Hide();
}


void scwin::UserProfileEdit::btnClose_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto ret = VisualStateManager::GoToState(SetupPasswordFlyout, "AllGood", false);
	OutputDebugString(ret ? L"Yes" : L"No");
	ViewModel->UserPwd = nullptr;
	PasswordSetupFlyout->Hide();
}


void scwin::UserProfileEdit::pwdFirst_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	safe_cast<PasswordBox^>(sender)->SelectAll();
}
