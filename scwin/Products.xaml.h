//
// Products.xaml.h
// Declaration of the Products class
//

#pragma once

#include "ProductViewModel.h"
#include "Products.g.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Products sealed
	{
	public:
		Products();


		property Command^ AddCommand;

		property ProductViewModel^ ViewModel;

	private:
		void GoBackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void InnerFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void InnerFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);

		void lnkAddProducts_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void ExecuteAddCommand(Object^ data);
		bool CanExecuteAddCommand(Object^ data);


	};
}
