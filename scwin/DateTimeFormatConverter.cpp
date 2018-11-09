#include "pch.h"
#include "DateTimeFormatConverter.h"

using namespace Windows::Globalization::DateTimeFormatting;

DateTimeFormatConverter::DateTimeFormatConverter()
{
}

Platform::Object ^ scwin::DateTimeFormatConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto boxed = dynamic_cast<IBox<DateTime>^>(value);
	if (boxed == nullptr)
		return L"";
	auto formatter = ref new DateTimeFormatter(DateTimeFormat);
	return formatter->Format(boxed->Value);
}

Platform::Object ^ scwin::DateTimeFormatConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
