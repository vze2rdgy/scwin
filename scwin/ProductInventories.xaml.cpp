//
// ProductInventories.xaml.cpp
// Implementation of the ProductInventories class
//

#include "pch.h"
#include "ProductInventories.xaml.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

ProductInventories::ProductInventories()
{
	ViewModel = ref new ProductInventoryViewModel();
	InitializeComponent();
}

void scwin::ProductInventories::OnViewLoaded(Object ^ sender, RoutedEventArgs ^ eargs)
{
}

void scwin::ProductInventories::btnAddStorageLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->EditedStorageLocation = ref new ProductInventoryLocation();
	//ViewModel->EditedStorageLocation->Address = ref new Address(); 
	InventoryEditControl->DataContext = ViewModel;
	InventoryEditControl->ContentTemplate = (DataTemplate^)Resources->Lookup("ProductInventoryAddLocation");
	ViewModel->IsPaneOpen = true;
}


void scwin::ProductInventories::hlnkLocationName_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto selItem = ((HyperlinkButton^)sender)->DataContext;
	if (selItem != nullptr)
	{
		auto lv = (ListViewItem^)listOfInvLocations->ContainerFromItem(selItem);
		if (lv != nullptr)
		{
			lv->IsSelected = true;
		}

		ViewModel->EditedStorageLocation = ((ProductInventoryLocation^)selItem)->Clone();
		ViewModel->EditedStorageLocation->ObjectState = DataState::Edited;
		//if (ViewModel->EditedStorageLocation->Address == nullptr)
		//	ViewModel->EditedStorageLocation->Address = ref new Address();
		InventoryEditControl->DataContext = ViewModel;
		InventoryEditControl->ContentTemplate = (DataTemplate^)Resources->Lookup("ProductInventoryEditLocation");
		ViewModel->IsPaneOpen = true;
	}
}


void scwin::ProductInventories::btnStorageLocationClosePane_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->IsPaneOpen = false;
}


void scwin::ProductInventories::btnPaneCancelStoreLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->IsPaneOpen = false;
}

void scwin::ProductInventories::chkEnterAddress_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->EditedStorageLocation->Address = ref new Address();
}


void scwin::ProductInventories::chkEnterAddress_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ViewModel->EditedStorageLocation->ObjectState == DataState::Edited
		&& ViewModel->EditedStorageLocation->Address != nullptr
		&& ViewModel->EditedStorageLocation->Address->IsValid
		)
	{
		using namespace Windows::UI::Popups;
		auto dlg = ref new MessageDialog("Unchecking this check box may result in loosing previously saved address. Do you want to continue ?", "Remove Address");
		dlg->Commands->Append(ref new UICommand("Yes", ref new UICommandInvokedHandler(this, &ProductInventories::OnAcceptUncheckAddressCheckBox)));
		dlg->Commands->Append(ref new UICommand("No"));
	}
	else
	{
		ViewModel->EditedStorageLocation->Address = nullptr;
	}
}

void scwin::ProductInventories::OnAcceptUncheckAddressCheckBox(Windows::UI::Popups::IUICommand ^ cmd)
{
	ViewModel->EditedStorageLocation->Address = nullptr;
}


void scwin::ProductInventories::listOfInvLocations_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	ViewModel->SelectedIndex = listOfInvLocations->SelectedIndex;
}


void scwin::ProductInventories::btnNewProductInventory_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void scwin::ProductInventories::pnlProductInventory_Expanded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->FetchProductInventories();
}
