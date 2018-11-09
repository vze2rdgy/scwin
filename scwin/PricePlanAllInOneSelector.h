#pragma once

namespace scwin
{

	public ref class PricePlanAllInOneSelector sealed : public DataTemplateSelector
	{
	public:
		PricePlanAllInOneSelector();

	protected:
		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;

	};

}
