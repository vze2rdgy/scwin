#include "pch.h"
#include "ProductInventoryDataTemplateSelector.h"


ProductInventoryDataTemplateSelector::ProductInventoryDataTemplateSelector(ResourceDictionary^ dictionary) : dictionary(dictionary)
{
}

DataTemplate ^ scwin::ProductInventoryDataTemplateSelector::SelectTemplateCore(Object ^ item)
{
	return SelectTemplateCore(item, nullptr);
}

DataTemplate ^ scwin::ProductInventoryDataTemplateSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	if (dynamic_cast<ProductInventoryLocation^>(item) != nullptr)
	{
		return (DataTemplate^)dictionary->Lookup("ProductInventoryManageTemplate");
	}
	return nullptr;
}

