#pragma once

using namespace Windows::UI::Xaml::Data;

namespace scwin
{

	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PropertyNotSetConverter sealed : public IValueConverter
	{
	public:
		PropertyNotSetConverter();
		virtual Platform::Object^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};


}
