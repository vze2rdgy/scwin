#include "pch.h"
#include "StringLengthConverters.h"

using namespace Windows::UI::Xaml;
using namespace scwin;

StringLengthVisibilityConverter::StringLengthVisibilityConverter()
{
}

Platform::Object ^ scwin::StringLengthVisibilityConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	if (dynamic_cast<String^>(value) == nullptr)
	{
		return Visibility::Collapsed;
	}
	String^ s = (String^)value;
	if (s->Length() == 0)
	{
		return Visibility::Collapsed;
	}
	return Visibility::Visible;
}

Platform::Object ^ scwin::StringLengthVisibilityConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new NotImplementedException("Not implemented");
}

////////////////////////////////////////StringLengthEnableConverter/////////////////////////////


StringLengthEnableConverter::StringLengthEnableConverter()
{
}

Platform::Object ^ scwin::StringLengthEnableConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	if (dynamic_cast<String^>(value) == nullptr)
	{
		return false;
	}
	String^ s = (String^)value;
	if (s->Length() == 0)
	{
		return false;
	}
	return true;
}

Platform::Object ^ scwin::StringLengthEnableConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new NotImplementedException("Not implemented");
}
