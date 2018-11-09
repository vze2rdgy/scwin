//
// AppTabView.xaml.h
// Declaration of the AppTabView class
//

#pragma once

#include "LayoutAwarePage.h"
#include "scMainViewModel.h"
#include "AppTabView.g.h"
#include <map>

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AppTabView sealed
	{
		typedef std::map<String^, TypeName> NAVPAGEITEMS;
		typedef std::map<String^, NAVPAGEITEMS> NAVITEMS;
		NAVITEMS NavItemToPageViewMap;

	public:
		AppTabView(scMainViewModel^ vm);

		property scMainViewModel^ ViewModel;

		property Windows::UI::Xaml::Controls::Frame^ AppFrame
		{
			Windows::UI::Xaml::Controls::Frame^ get();
		}

	private:
		void InitializeNavigationRequirements();
		tuple<String^, String^> GetHeaderAndPage(TypeName typeName);
		void SystemNavigationManager_BackRequested(Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);
		void NavigateToPage(String^ navItemHeader, String^ navItemTitle);
		void NavManager_BackRequested(Object^ sender, BackRequestedEventArgs^ e);
		void btnRemoveMesage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void NavView_SelectionChanged(Windows::UI::Xaml::Controls::NavigationView^ sender, Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs^ args);
		void frame_Navigating(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e);
	};
}
