//
// SalesOrderView.xaml.h
// Declaration of the SalesOrderView class
//

#pragma once

#include "ChoooseACustomer.xaml.h"
#include "ChooseAProduct.xaml.h"
#include "DiscountTypeToBooleanConverter.h"
#include "DiscountTypeToTextConverter.h"
#include "SalesOrderView.g.h"
#include "SaleOrderViewModel.h"
#include "CustomerViewModel.h"


using namespace sggraphs;


namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SalesOrderView sealed
	{
		IVector<Product^>^ plist;
	public:
		SalesOrderView();

		property SaleOrderViewModel^ ViewModel;

		property CustomerViewModel^ CustViewModel 
		{
			CustomerViewModel^ get()
			{
				return ViewModel->CustomerViewmodel;
			}
		}

		property IVector<Product^>^ ProductList
		{
			IVector<Product^>^ get() 
			{ 
				return plist; 
			}
		private:
			void set(IVector<Product^>^ val) { plist = val; }
		}

		void OnNavigatedTo(NavigationEventArgs^ e) override;

		void OnNavigatingFrom(NavigatingCancelEventArgs^ e) override;

		property IVector<TransactionFlowItem^>^ OrderFlow;


	private:
		void MainCommandBar_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);

		void FindProduct(FrameworkElement^ sender, String^ text);
		void txtProductName_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void txtProductName_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void ChooseAProduct_ProductSelected(scwin::ChooseAProduct^ sender, scwin::ProductWithInventoryWrapper^ productWrapper);
		void flyExpeditedView_Opening(Platform::Object^ sender, Platform::Object^ e);
		void flyExpeditedView_Closed(Platform::Object^ sender, Platform::Object^ e);
		void btnAddOrderItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void dtOrderExpectedDate_DateChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::DatePickerValueChangedEventArgs^ e);
		void txtOrderHeaderDiscountType_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnRemoveOrderLine_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ButtonRevertOrder_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ButtonSaveOrder_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void DisplayOrderStatusHistory();

	};
}
