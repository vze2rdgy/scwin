#include "pch.h"
#include "NullVisibilityConverter.h"


NullToVisibilityConverter::NullToVisibilityConverter()
{
}

Platform::Object ^ scwin::NullToVisibilityConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	bool inverse = false;
	if (parameter != nullptr)
	{
		auto bstr = dynamic_cast<String^>(parameter);
		if (bstr != nullptr)
			inverse = bstr == "True" ? true : false;
	}
	if (value == nullptr)
	{
		return inverse ? Visibility::Visible : Visibility::Collapsed;
	}
	else
	{
		return inverse ? Visibility::Collapsed : Visibility::Visible;
	}
}

Platform::Object ^ scwin::NullToVisibilityConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
