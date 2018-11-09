#pragma once

namespace scwin
{

	public ref class StatusMessageTemplateSelector sealed : public DataTemplateSelector
	{
	public:
		StatusMessageTemplateSelector(ResourceDictionary^ controlResources);

	private:
		WeakReference controlResources;

	protected:
		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	};

}