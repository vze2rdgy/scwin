//
// UserRoles.xaml.cpp
// Implementation of the UserRoles class
//

#include "pch.h"
#include "UserRoles.xaml.h"

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

using namespace std;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

UserRoles::UserRoles()
{
	vm = ref new RoleViewModel();

	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::UserRoles::OnLoaded);
	SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &scwin::UserRoles::OnSizeChanged);

}

RoleViewModel^ UserRoles::ViewModel::get()
{
	return vm;
}


void scwin::UserRoles::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
}


void scwin::UserRoles::OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
}

void scwin::UserRoles::listOfRoles_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void scwin::UserRoles::btnEdit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void scwin::UserRoles::hlinkRoleName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto dc = (UserRole^)((FrameworkElement^)sender)->Tag;
	if (dc != nullptr)
	{
		ViewModel->SelectedRole = dc;
		ViewModel->EditedRole = ViewModel->SelectedRole->Clone();
	}
	if (!ViewModel->IsPaneOpen)
	{
		ViewModel->IsPaneOpen = true;
	}
}

void scwin::UserRoles::btnRolePaneClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->IsPaneOpen = false;
}


void scwin::UserRoles::splitPane_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args)
{
}


void scwin::UserRoles::listOfRoles_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	ViewModel->SelectedIndex = listOfRoles->SelectedIndex;
}
