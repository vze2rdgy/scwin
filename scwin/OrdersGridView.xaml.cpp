//
// OrdersGridView.xaml.cpp
// Implementation of the OrdersGridView class
//

#include "pch.h"
#include "OrdersGridView.xaml.h"
#include "SalesOrderView.xaml.h"

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

OrdersGridView::OrdersGridView()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::OrdersGridView::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::OrdersGridView::OnUnloaded);


}


void scwin::OrdersGridView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);
	auto vm = dynamic_cast<SaleOrderViewModel^>(e->Parameter);
	ViewModel = vm;
	ViewModel->PopulateOrders();
}

void scwin::OrdersGridView::OnNavigatedFrom(NavigationEventArgs ^ e)
{
	Page::OnNavigatedFrom(e);
	ViewModel->SaveState();
}

void scwin::OrdersGridView::btnOrderGridViewType_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ViewModel->IsOrderviewInListViewMode = !ViewModel->IsOrderviewInListViewMode;
}


void scwin::OrdersGridView::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	for (auto str : ViewModel->SortSelections)
	{
		auto mnuItem = ref new MenuFlyoutItem();
		mnuItem->Text = str;
		mnuItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::OrdersGridView::mnuOrderSort_Clicked);
		mnuOrderSorts->Items->Append(mnuItem);
	}
}


void scwin::OrdersGridView::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->SaveState();
}


void scwin::OrdersGridView::mnuOrderSort_Clicked(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	auto mnuItem = safe_cast<MenuFlyoutItem^>(sender);
	ViewModel->CurrentSortSelection = mnuItem->Text;
}



void scwin::OrdersGridView::btnEditOrder_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Frame->Navigate(TypeName(SalesOrderView::typeid), ViewModel);
}
