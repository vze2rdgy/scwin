//
// Products.xaml.cpp
// Implementation of the Products class
//

#include "pch.h"
#include "Products.xaml.h"
#include "ProductGridView.xaml.h"
#include "ProductView.xaml.h"

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

Products::Products()
{
	InitializeComponent();


	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Products::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Products::OnUnloaded);

	ViewModel = ref new ProductViewModel();

	AddCommand = ref new Command(
		ref new ExecuteAction(this, &scwin::Products::ExecuteAddCommand),
		ref new CanExecuteFunc(this, &scwin::Products::CanExecuteAddCommand)
	);



	InnerFrame->Navigate(ProductGridView::typeid, ViewModel);

}

void Products::GoBackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (InnerFrame->CanGoBack)
	{
		InnerFrame->GoBack();
	}
}


void Products::InnerFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	GoBackButton->IsEnabled = InnerFrame->CanGoBack;
}


void Products::InnerFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	auto grect = ref new RectangleGeometry();
	grect->Rect = Rect(Point(), e->NewSize);
	InnerFrame->Clip = grect;
}

void Products::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->SaveState();
}


void Products::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
}


void scwin::Products::lnkAddProducts_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}

void scwin::Products::ExecuteAddCommand(Object ^ data)
{
	auto pvm = this->ViewModel;
	pvm->Initialize(nullptr);
	Frame->Navigate(TypeName(ProductView::typeid), pvm);
}

bool scwin::Products::CanExecuteAddCommand(Object ^ data)
{
	return true;
}

