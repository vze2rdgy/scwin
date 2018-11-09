#include "pch.h"
#include "ByteToImageConverter.h"
#include "ImageHelpers.h"


ByteToImageConverter::ByteToImageConverter()
{
}

Platform::Object ^ scwin::ByteToImageConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto imageBytes = safe_cast<Array<byte>^>(value);
	return ImageHelpers::ConvertToImage(imageBytes).get();
}

Platform::Object ^ scwin::ByteToImageConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

