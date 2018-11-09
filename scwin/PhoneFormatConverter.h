#pragma once

using namespace Windows::UI::Xaml::Data;

namespace scwin
{

	public ref class PhoneFormatConverter sealed : IValueConverter
	{
	public:
		PhoneFormatConverter();

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};

}