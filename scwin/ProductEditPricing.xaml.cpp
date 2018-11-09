//
// ProductEditPricing.xaml.cpp
// Implementation of the ProductEditPricing class
//

#include "pch.h"
#include "ProductEditPricing.xaml.h"
#include "Enumerable.h"

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
using namespace Windows::Globalization;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

ProductEditPricing::ProductEditPricing()
{

	auto selector = ref new PricePlanDetailsTemplateSelector(Resources);
	selector->EditableView = false;
	Resources->Insert(L"PricePlanDetailsReadonlyTemplateSelector", selector);

	selector = ref new PricePlanDetailsTemplateSelector(Resources);
	selector->EditableView = true;
	Resources->Insert(L"PricePlanDetailsEditTemplateSelector", selector);

	InitializeComponent();

	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductEditPricing::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductEditPricing::OnUnloaded);
}

void scwin::ProductEditPricing::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->LoadProductDataForPanel(2);
	ViewModel->PricePlan->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::ProductEditPricing::OnPropertyChanged);
}

void scwin::ProductEditPricing::OnApplyTemplate()
{
	UserControl::OnApplyTemplate();
}

void scwin::ProductEditPricing::btnPricePlanEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ViewModel->SetupProductSpecifics(2);
	ViewModel->EditPanel = 2;
	lastViewState = "EditState";
	GOTOVIEWSTATE(this, lastViewState);
}


void scwin::ProductEditPricing::btnPricePlanSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	WeakReference wr(this);
	ViewModel->EditPanel = -1;
	create_task(ViewModel->SaveProductDataForPanel(2))
		.then([wr](int code)
	{
		auto ths = wr.Resolve<ProductEditPricing>();
		if (ths->ViewModel->PricePlan->PricePlan == nullptr)
			ths->lastViewState = "NoDataState";
		else
			ths->lastViewState = "ViewOnlyState";
		GOTOVIEWSTATE(ths, ths->lastViewState);
	});
}


void scwin::ProductEditPricing::btnPricePlanCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ViewModel->EditPanel = -1;
	if (propChangedTok.Value == 0)
	{
		//propChangedTok = ViewModel->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::ProductEditPricing::OnPropertyChanged);
		ViewModel->PricePlan->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::ProductEditPricing::OnPropertyChanged);
	}
	
	ViewModel->CancelProductDataForPanel(2);
	if (ViewModel->PricePlan->PricePlan == nullptr)
		lastViewState = "NoDataState";
	else
		lastViewState = "ViewOnlyState";
	GOTOVIEWSTATE(this, lastViewState);
}


void scwin::ProductEditPricing::OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
	//auto ctrl = this;
	//PricePlanContent->Width = ctrl->ActualWidth;
}




void scwin::ProductEditPricing::btnRemoveTierPrice_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	WeakReference wr(this);
	MessageDialog^ dlg = ref new MessageDialog("Do you want to remove this tiered price plan item ?", "Remove Price Plan");
	dlg->Commands->Append(ref new UICommand("Yes", nullptr, 0));
	dlg->Commands->Append(ref new UICommand("No", nullptr, 1));
	dlg->CancelCommandIndex = 1;
	dlg->DefaultCommandIndex = 1;
	create_task(dlg->ShowAsync())
		.then([wr, sender](IUICommand^ cmd)
	{
		if ((int)cmd->Id == 0)
		{
			auto priceItem = dynamic_cast<PricePlanTierDetails^>(((Button^)sender)->DataContext);
			auto ths = wr.Resolve<ProductEditPricing>();
			if (ths->ViewModel->PricePlan != nullptr && ths->ViewModel->PricePlan->PricePlan != nullptr)
			{
				int index = Enumerable::FindIndex(ths->ViewModel->PricePlan->PriceDetails, (IPricePlanDetails^)priceItem);
				if (index >= 0)
					ths->ViewModel->PricePlan->PriceDetails->RemoveAt(index);
			}
		}
	});
}



void scwin::ProductEditPricing::btnPricePlanLookup_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	PricePlanLookup^ lookup = ref new PricePlanLookup();
	lookup->ShowAsync();
}


void scwin::ProductEditPricing::OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e)
{
	if (e->PropertyName == "PricePlan")
	{
		if (ViewModel->PricePlan != nullptr && ViewModel->PricePlan->PricePlan != nullptr)
		{
			lastViewState = "ViewOnlyState";
		}
		else
		{
			lastViewState = "NoDataState";
		}
		GOTOVIEWSTATE(this, lastViewState)
	}
}


void scwin::ProductEditPricing::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	ViewModel->UnloadProductDataForPanel(2);
	//ViewModel->PropertyChanged -= propChangedTok;
	//propChangedTok.Value = 0;
}
