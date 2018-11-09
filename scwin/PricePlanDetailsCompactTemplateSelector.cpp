#include "pch.h"
#include "PricePlanDetailsCompactTemplateSelector.h"

PricePlanDetailsCompactTemplateSelector::PricePlanDetailsCompactTemplateSelector(ResourceDictionary^ controlResources)
{
	this->controlResources = controlResources;
}


DataTemplate ^ scwin::PricePlanDetailsCompactTemplateSelector::SelectTemplateCore(Object ^ item)
{
	return SelectTemplateCore(item, nullptr);
}

DataTemplate ^ scwin::PricePlanDetailsCompactTemplateSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	if (item == nullptr)
		return nullptr;

	if (dynamic_cast<PricePlanStandardDetails^>(item) != nullptr)
	{
		return (DataTemplate^)controlResources->Lookup("PricePlanStandDetails");
	}
	else
	{
		return (DataTemplate^)controlResources->Lookup("PricePlanTierDetails");
	}
}

