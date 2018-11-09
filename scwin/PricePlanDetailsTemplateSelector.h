#pragma once

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace scwin
{

	public ref class PricePlanDetailsTemplateSelector sealed :
		public DataTemplateSelector
	{
	private:
		ResourceDictionary^ controlResources;

	protected:
		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	public:
		PricePlanDetailsTemplateSelector(ResourceDictionary^ controlResources);

		property bool EditableView;


	};

	

}
