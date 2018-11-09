#include "pch.h"
#include "ProductImageAvailabilityConverte.h"


ProductImageAvailabilityConverter::ProductImageAvailabilityConverter()
{
}

Platform::Object ^ scwin::ProductImageAvailabilityConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	Product^ product = dynamic_cast<Product^>(value);
	if (product == nullptr || product->TitleImage == nullptr)
	{
		if (isInverse)
			return Visibility::Visible;
		else
			return Visibility::Collapsed;
	}
	return Visibility::Visible;
}

Platform::Object ^ scwin::ProductImageAvailabilityConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
