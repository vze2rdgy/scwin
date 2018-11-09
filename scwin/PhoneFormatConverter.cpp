#include "pch.h"
#include "PhoneFormatConverter.h"


PhoneFormatConverter::PhoneFormatConverter()
{
}

Platform::Object ^ scwin::PhoneFormatConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	return value;
}

Platform::Object ^ scwin::PhoneFormatConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	return value;
}
