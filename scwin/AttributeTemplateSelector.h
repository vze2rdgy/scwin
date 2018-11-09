#pragma once

namespace scwin
{

	public ref class AttributeTemplateSelector sealed :
		public DataTemplateSelector
	{
	private:
		ResourceDictionary^ controlResources;

	protected:
		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	public:
		AttributeTemplateSelector(ResourceDictionary^ controlResources);

		property bool EditableView;

	};


}
