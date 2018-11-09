//
// Customers.xaml.h
// Declaration of the Customers class
//

#pragma once

#include "LayoutAwarePage.h"
#include "Customers.g.h"
#include "CustomerViewModel.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Customers sealed
	{
	public:
		Customers();

		property CustomerViewModel^ ViewModel;

	private:
		void GoBackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void InnerFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void InnerFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

		void lnkAddCustomer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	};
}
