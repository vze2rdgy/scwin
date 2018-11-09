//
// Orders.xaml.cpp
// Implementation of the Orders class
//

#include "pch.h"
#include "Orders.xaml.h"
#include "OrdersGridView.xaml.h"
#include "ChoooseACustomer.xaml.h"
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

Orders::Orders()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Orders::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Orders::OnUnloaded);

	ViewModel = ref new SaleOrderViewModel();

}


void scwin::Orders::GoBackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (InnerFrame->CanGoBack)
	{
		InnerFrame->GoBack();
	}

}




void scwin::Orders::InnerFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	GoBackButton->IsEnabled = InnerFrame->CanGoBack;

}


void scwin::Orders::InnerFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	auto grect = ref new RectangleGeometry();
	grect->Rect = Rect(Point(), e->NewSize);
	InnerFrame->Clip = grect;
}


void scwin::Orders::lnkAddOrders_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//InnerFrame->Navigate(TypeName(ChoooseACustomer::typeid), ViewModel);
	ViewModel->UpdateCurrentOrder(ViewModel->NewOrderWrapper());
	Frame->Navigate(TypeName(SalesOrderView::typeid), ViewModel);
}


void scwin::Orders::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	InnerFrame->Navigate(TypeName(OrdersGridView::typeid), ViewModel);
}


void scwin::Orders::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
}
