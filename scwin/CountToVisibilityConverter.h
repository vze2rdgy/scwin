#pragma once

namespace scwin
{

	public ref class CountToVisibilityConverter sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		CountToVisibilityConverter();

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);

		property bool IsInverse;
	};

}