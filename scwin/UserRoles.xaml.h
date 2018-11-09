//
// UserRoles.xaml.h
// Declaration of the UserRoles class
//

#pragma once

#include "UserRoles.g.h"
#include "UserRole.h"
#include "AdminViewModel.h"
#include "UserRoleControl.xaml.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserRoles sealed
	{
	private:
		RoleViewModel ^ vm;

	public:
		UserRoles();
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);

		property RoleViewModel^ ViewModel
		{
			RoleViewModel^ get();
		}

	private:
		void listOfRoles_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnEdit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void hlinkRoleName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnRolePaneClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void splitPane_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args);
		void listOfRoles_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
	};
}
