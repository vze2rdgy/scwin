#include "pch.h"
#include "NullToBooleanConverter.h"


NullToBooleanConverter::NullToBooleanConverter()
{
}

Platform::Object ^ scwin::NullToBooleanConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto b = value != nullptr ? true : false;
	return this->isInverse ? !b : b;
}

Platform::Object ^ scwin::NullToBooleanConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
