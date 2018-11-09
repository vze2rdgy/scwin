//
// ProductInventories.xaml.h
// Declaration of the ProductInventories class
//

#pragma once

#include "ProductInventories.g.h"
#include "ProductInventoryViewModel.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductInventories sealed
	{
	public:
		
		ProductInventories();

		property ProductInventoryViewModel^ ViewModel;

	private:
		void OnViewLoaded(Object^ sender, RoutedEventArgs^ eargs);
		void btnAddStorageLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void hlnkLocationName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnStorageLocationClosePane_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPaneCancelStoreLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void chkEnterAddress_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void chkEnterAddress_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnAcceptUncheckAddressCheckBox(Windows::UI::Popups::IUICommand^ cmd);
		void listOfInvLocations_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void btnNewProductInventory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void pnlProductInventory_Expanded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
