//
// AppNavigator.xaml.h
// Declaration of the AppNavigator class
//

#pragma once

#include "AppNavigator.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AppNavigator sealed
	{
		static DependencyProperty^ selectedSubmenuItemsProperty;
		static DependencyProperty^ selectedSubmenuItemProperty;

	public:

		static void Register();

		AppNavigator();

		static property DependencyProperty^ SelectedSubmenuItemsProperty
		{
			DependencyProperty^ get() { return selectedSubmenuItemsProperty; }
		}

		static property DependencyProperty^ SelectedSubmenuItemProperty
		{
			DependencyProperty^ get() { return selectedSubmenuItemProperty; }
		}

		property IObservableVector<NavMenuItem^>^ AppNavIcons;

		property IObservableVector<NavMenuItem^>^ SelectedSubmenuItems
		{
			IObservableVector<NavMenuItem^>^ get();
			void set(IObservableVector<NavMenuItem^>^);
		}

		property NavMenuItem^ SelectedSubmenuItem
		{
			NavMenuItem^ get();
			void set(NavMenuItem^);
		}

	protected:
		void OnKeyDown(KeyRoutedEventArgs ^ e) override;

	private:
		void LogoutNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void SettingsNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void gvNavIcons_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void btnForNavBar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void BuildSubmenus(String^ navHeaderName, String^ lastSelectedSubmenuItem = nullptr);
		void KeyDownHandler(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	};
}
