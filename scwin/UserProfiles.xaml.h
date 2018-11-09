//
// UserProfiles.xaml.h
// Declaration of the UserProfiles class
//

#pragma once

#include "UserProfiles.g.h"
#include "UserProfile.h"
#include "AdminViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserProfiles sealed
	{
		EventRegistrationToken viewPropChangedToken;
		//Control^ lastTapped;
		bool directResetPwdReq = false;


	public:
		UserProfiles();

		property UserViewModel^ ViewModel;

	private:
		void OnDataContextChanged(Windows::UI::Xaml::FrameworkElement ^sender, Windows::UI::Xaml::DataContextChangedEventArgs ^args);
		void OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e);
		//void listOfUsers_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		//void txtSearchUsers_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		//void txtSearchUsers_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnPubsubMessage(Object ^ sender, PubSubMessageIds msgId, Object ^ data);
		void splitPane_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args);
		void listOfUsers_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void hlinkUserName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnUserProfilePaneClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cmbRolesFilter_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void btnUserProfileEditCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnAddUserProfile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnkResetPassword_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void appbtnResetPassword_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnInviteNewUser_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
