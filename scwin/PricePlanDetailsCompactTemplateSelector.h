#pragma once

namespace scwin
{

	public ref class PricePlanDetailsCompactTemplateSelector sealed :
		public DataTemplateSelector
	{
	private:
		ResourceDictionary^ controlResources;

	protected:
		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	public:
		PricePlanDetailsCompactTemplateSelector(ResourceDictionary^ controlResources);
	};


}
