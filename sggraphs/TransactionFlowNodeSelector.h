#pragma once

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace sggraphs
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TransactionFlowNodeSelector sealed : public DataTemplateSelector
	{
	private:
		ResourceDictionary^ controlResources;

	public:
		TransactionFlowNodeSelector(ResourceDictionary^ controlResources);


	protected:
		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	};

}