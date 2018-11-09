#include "pch.h"
#include "PricePlanAllInOneSelector.h"


PricePlanAllInOneSelector::PricePlanAllInOneSelector()
{
}

DataTemplate ^ scwin::PricePlanAllInOneSelector::SelectTemplateCore(Object ^ item)
{
	return SelectTemplateCore(item, nullptr);
}

DataTemplate ^ scwin::PricePlanAllInOneSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	PricePlanViewModelFacade^ priceList = nullptr;
	if (item == nullptr || nullptr == (priceList = dynamic_cast<PricePlanViewModelFacade^>(item)))
		return (DataTemplate^)App::Current->Resources->Lookup("UnknownPricePlanAllInOneTemplate");
		// check first price in the list
	if (!priceList->IsTieredPricePlan)
		return (DataTemplate^)App::Current->Resources->Lookup("StdPricePlanAllInOneTemplate");
	else
		return (DataTemplate^)App::Current->Resources->Lookup("TierPricePlanAllInOneTemplate");
}
