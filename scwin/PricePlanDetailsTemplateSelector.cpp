#include "pch.h"
#include "PricePlanDetailsTemplateSelector.h"


DataTemplate ^ scwin::PricePlanDetailsTemplateSelector::SelectTemplateCore(Object ^ data)
{
	return SelectTemplateCore(data, nullptr);
}

DataTemplate ^ scwin::PricePlanDetailsTemplateSelector::SelectTemplateCore(Object ^ data, DependencyObject ^ container)
{
	if (data == nullptr)
		return nullptr;

	if (dynamic_cast<PricePlanStandardDetails^>(data) != nullptr)
	{
		if (EditableView)
			return (DataTemplate^)controlResources->Lookup("StandardPriceEditView");
		else
			return (DataTemplate^)controlResources->Lookup("StandardPriceReadView");
	}
	else
	{
		if (EditableView)
			return (DataTemplate^)controlResources->Lookup("TieredPriceEditView");
		else
			return (DataTemplate^)controlResources->Lookup("TieredPriceReadView");
	}
}

PricePlanDetailsTemplateSelector::PricePlanDetailsTemplateSelector(ResourceDictionary^ resources) : controlResources(resources)
{

}
