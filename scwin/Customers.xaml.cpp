//
// Customers.xaml.cpp
// Implementation of the Customers class
//

#include "pch.h"
#include "Customers.xaml.h"
#include "COrgGridView.xaml.h"
#include "CustomerOrg.h"
#include "COrgGridView.xaml.h"

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

Customers::Customers()
{
	InitializeComponent();

	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Customers::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Customers::OnUnloaded);

	ViewModel = ref new CustomerViewModel();
}


void scwin::Customers::GoBackButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void scwin::Customers::InnerFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
}


void scwin::Customers::InnerFrame_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
}

void scwin::Customers::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->SaveState();
}


void scwin::Customers::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
}


void scwin::Customers::lnkAddCustomer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	create_task(CustomerDtosGroupedBindable::New())
		.then([this](CustomerDtosGroupedBindable^ bindable)
	{
		auto f = [this, bindable]()
		{
			//TimeSpan span;
			//span.Duration = 5000000;
			//ConnectedAnimationService::GetForCurrentView()->DefaultDuration = span;
			//auto connectedAnimation = ConnectedAnimationService::GetForCurrentView()
			//	->PrepareToAnimate("AddCustomerLink", ths->lnkAddCustomer);
			ViewModel->Current = bindable;
			ViewModel->CurrentAction = DataState::Added;
			//this->Frame->Navigate(TypeName(COrgEditorView::typeid), ViewModel);
		};

		if (Dispatcher->HasThreadAccess)
		{
			f();
		}
		else
		{
			Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(f));
		}
	});
}
