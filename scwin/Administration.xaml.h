//
// Administration.xaml.h
// Declaration of the Administration class
//

#pragma once

#include "LayoutAwarePage.h"
#include "AdminViewModel.h"
#include "MultiViewControl.xaml.h"
#include "UserRoles.xaml.h"
#include "UserProfiles.xaml.h"
#include "ProductFamilies.xaml.h"
#include "Administration.g.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Administration sealed
	{
	private:
		EventRegistrationToken rolevmChangedtoken;

	private:

	public:
		Administration();
		virtual ~Administration();
		property AdminViewModel^ ViewModel;

	protected:
		virtual void OnNavigatedTo(NavigationEventArgs^ e) override ;

	private:
		void lvAdminRoles_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void chkEditRole_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void AdminPivot_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
	};
}
