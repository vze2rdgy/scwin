#include "pch.h"
#include "PricePlanViewModelFacade.h"
#include "cpplinq.hpp"

#define STANDARDPLANTYPE "Standard"
#define TIEREDPLANTYPE "Tiered"

PricePlanViewModelFacade::PricePlanViewModelFacade(Product^ prod) : product(prod)
{
	auto planTypes = ref new Vector<PricePlanType^>();
	auto pt = ref new PricePlanType();
	pt->Name = STANDARDPLANTYPE;
	pt->IsSelected = true;
	planTypes->Append(pt);
	pt = ref new PricePlanType();
	pt->Name = TIEREDPLANTYPE;
	pt->IsSelected = false;
	planTypes->Append(pt);
	PlanTypes = planTypes->GetView();
	LoadPricePlan();
}

scwin::PricePlanViewModelFacade::~PricePlanViewModelFacade()
{
	//TODO clean up class data.
}



void scwin::PricePlanViewModelFacade::RaisePropertyChanged(String ^ propName)
{
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([=]() 
	{ 
		PropertyChanged(this, ref new PropertyChangedEventArgs(propName)); 
	})); 
}

void scwin::PricePlanViewModelFacade::LoadPricePlanAsync()
{
	create_task([=]() {LoadPricePlan(); });
}

void scwin::PricePlanViewModelFacade::LoadPricePlan()
{
	if (product == nullptr) return;

	if (StringHelpers::IsNullOrEmpty(product->PricePlanId)) return;

	using namespace cpplinq;
	auto ths = this;
	auto facade = this; // wr.Resolve<PricePlanViewModelFacade>();
	auto pplan = LOCSVC->GetProductPricingPlan(facade->product->PricePlanId);
	facade->PricePlan = pplan;
	if (pplan != nullptr)
	{
		auto stdplan = LOCSVC->GetProductStandardPricePlan(pplan->Id);
		if (stdplan != nullptr)
		{
			ths->PriceDetails = ref new Vector<IPricePlanDetails^>();
			ths->PriceDetails->Append(stdplan);
			auto planType = from(to_vector(ths->PlanTypes)) >> where([](PricePlanType^ type)
			{
				return type->Name == STANDARDPLANTYPE;
			}) >> first();
			planType->IsSelected = true;
			ths->IsTieredPricePlan = false;
			return;
		}
		auto tierdets = LOCSVC->GetProductTierPricePlans(pplan->Id);
		if (tierdets != nullptr)
		{
			ths->PriceDetails = ref new Vector<IPricePlanDetails^>();
			for (auto det : tierdets)
			{
				ths->PriceDetails->Append(det);
			}
			auto planType = from(to_vector(ths->PlanTypes)) >> where([](PricePlanType^ type)
			{
				return type->Name == TIEREDPLANTYPE;
			}) >> first();
			planType->IsSelected = true;
			ths->IsTieredPricePlan = true;
		}
	}
}

void scwin::PricePlanViewModelFacade::PricePlanSelectionChanged(PricePlanType^ selected)
{
	IsTieredPricePlan = selected != nullptr && selected->Name == TIEREDPLANTYPE;

	//if (mementoPriceDetails == nullptr && PriceDetails != nullptr)
	//{
	//	mementoPriceDetails = ref new Vector<IPricePlanDetails^>();
	//	for (auto pd : PriceDetails)
	//	{
	//		mementoPriceDetails->Append(pd);
	//	}
	//}

	if (PriceDetails == nullptr)
		PriceDetails = ref new Vector<IPricePlanDetails^>();
	else
		PriceDetails->Clear();
	if (IsTieredPricePlan)
	{
		PriceDetails->Append(ref new PricePlanTierDetails());
	}
	else
	{
		PriceDetails->Append(ref new PricePlanStandardDetails());
	}
}

void scwin::PricePlanViewModelFacade::AddTierPriceWhenTapped(Platform::Object ^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e)
{
	if (PriceDetails == nullptr)
		PriceDetails = ref new Vector<IPricePlanDetails^>();
	PriceDetails->Append(ref new PricePlanTierDetails());
}

void scwin::PricePlanViewModelFacade::RemoveTierPriceWhenTapped(Platform::Object ^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e)
{
	auto dc = ((Button^)sender)->DataContext;
	PricePlanTierDetails^ dets = dynamic_cast<PricePlanTierDetails^>(dc);
	if (dets != nullptr)
	{

	}
}

String ^ scwin::PricePlanViewModelFacade::GetPricePlanType()
{
	if (priceDetails == nullptr || !priceDetails->Size)
	{
		return L"";
	}
	// get first item
	auto firstItem = priceDetails->GetAt(0);
	
	if (nullptr != dynamic_cast<PricePlanStandardDetails^>(firstItem))
		return "Standard Pricing";

	// TODO: when more price plan types are necessary, this function may have more conditions.

	return "Tiered Pricing";
}

bool scwin::PricePlanViewModelFacade::ViewStateNoPricePlan()
{
	if (this->PricePlan == nullptr)
		return true;
	return false;
}
