//
// OrdersGridView.xaml.h
// Declaration of the OrdersGridView class
//

#pragma once

#include "CustomerViewControl.xaml.h"
#include "OrdersGridView.g.h"
#include "SaleOrderViewModel.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class OrdersGridView sealed
	{
	public:
		OrdersGridView();

		property SaleOrderViewModel^ ViewModel;

		property IObservableVector<OrderGroupedInfoList^>^ GroupedOrders;

	protected:
		void OnNavigatedTo(NavigationEventArgs^ e) override;
		void OnNavigatedFrom(NavigationEventArgs^ e) override;

	private:
		void btnOrderGridViewType_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void mnuOrderSort_Clicked(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void btnEditOrder_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
