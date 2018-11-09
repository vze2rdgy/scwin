#pragma once

namespace scwin
{

	public ref class ProductInventoryDataTemplateSelector sealed : public DataTemplateSelector
	{
		ResourceDictionary^ dictionary;

	public:
		ProductInventoryDataTemplateSelector(ResourceDictionary^ dictionary);
		DataTemplate^ SelectTemplateCore(Object^ item) override;
		DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	};

}