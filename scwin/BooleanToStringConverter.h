#pragma once


namespace scwin
{
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class BooleanToStringConverter sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		BooleanToStringConverter();

		// Inherited via IValueConverter
		virtual Platform::Object^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);

	};

}
