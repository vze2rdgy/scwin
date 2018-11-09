#pragma once

using namespace Windows::UI::Xaml::Data;

namespace scwin
{

	public ref class StringToUriConverter sealed : IValueConverter
	{
	public:
		StringToUriConverter();

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};

}