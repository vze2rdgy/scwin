#include "pch.h"
#include "PropertyNotSetConverter.h"


PropertyNotSetConverter::PropertyNotSetConverter()
{
}

// if the value argument is null, returns a string '[Not Set]'
Platform::Object ^ scwin::PropertyNotSetConverter::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
{
	if (value == nullptr)
	{
		if (parameter == nullptr)
			return "[Not Set]";
		else
		{
			return parameter;
		}
	}
	return value;
}

Platform::Object ^ scwin::PropertyNotSetConverter::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
{
	return nullptr;
}
