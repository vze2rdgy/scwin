#include "pch.h"
#include "CountToVisibilityConverter.h"


CountToVisibilityConverter::CountToVisibilityConverter()
{
}

Platform::Object ^ scwin::CountToVisibilityConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto size = dynamic_cast<IBox<UINT>^>(value);
	if (size == nullptr || size->Value == 0)
	{
		return IsInverse ? Visibility::Visible: Visibility::Collapsed;
	}
	return IsInverse ? Visibility::Collapsed : Visibility::Visible;
}

Platform::Object ^ scwin::CountToVisibilityConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
