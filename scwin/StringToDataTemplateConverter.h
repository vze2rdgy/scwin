#pragma once

namespace scwin
{

	public ref class StringToDataTemplateConverter sealed : IValueConverter
	{
	public:
		StringToDataTemplateConverter(ResourceDictionary^ resources);

		property ResourceDictionary^ Resources;

		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};

}