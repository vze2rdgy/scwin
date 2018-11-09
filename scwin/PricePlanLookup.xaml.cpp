//
// PricePlanLookup.xaml.cpp
// Implementation of the PricePlanLookup class
//

#include "pch.h"
#include "PricePlanLookup.xaml.h"

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

PricePlanLookup::PricePlanLookup()
{

	Resources->Insert("PricePlanDetailsCompactTemplateSelector", ref new PricePlanDetailsCompactTemplateSelector(Resources));


	InitializeComponent();



	WeakReference wr(this);
	create_task(scLocalDataService::Instance()->GetProductPricingPlansAsync())
		.then([wr](IVectorView<ProductPricePlan^>^ view)
	{
		auto ths = wr.Resolve<PricePlanLookup>();
		Vector<PricePlanDataItem^>^ v = ref new Vector<PricePlanDataItem^>();
		for (auto plan : view)
		{
			PricePlanDataItem^ ditem = ref new PricePlanDataItem();
			ditem->Plan = plan;
			v->Append(ditem);
			create_task(scLocalDataService::Instance()->GetPricePlanDetailsAsync(plan->Id))
				.then([ditem](GenericPriceDetailsList^ detlist)
			{
				ditem->DetailsList = detlist;
			});
		}
		ths->PricePlanitems = v->GetView();
	});
}

DEFINERAISEPROPCHANGEDBASIC(PricePlanDataItem)

DEFINERAISEPROPCHANGEDBASIC(PricePlanLookup)


void scwin::PricePlanLookup::AcceptButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	Hide();
}


void scwin::PricePlanLookup::CloseButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	Hide();
}
