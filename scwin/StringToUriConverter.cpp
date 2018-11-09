#include "pch.h"
#include "StringToUriConverter.h"


StringToUriConverter::StringToUriConverter()
{
}


Platform::Object ^ scwin::StringToUriConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto uri = dynamic_cast<Uri^>(value);
	if (uri != nullptr)
		return uri->DisplayIri;
	LogException("URI instance is invalid.");
	return L"";
}

Platform::Object ^ scwin::StringToUriConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto str = dynamic_cast<String^>(value);
	if (str != nullptr && !StringHelpers::IsNullOrEmpty(str))
		return ref new Uri(str);
	return nullptr;
}
