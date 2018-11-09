#include "pch.h"
#include "BrushConverter.h"

using namespace Windows::UI;
using namespace Windows::UI::Xaml::Media;

sgCore::BrushConverter::BrushConverter()
{
}

Object ^ sgCore::BrushConverter::Convert(Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Object ^parameter, String ^language)
{
	return ref new SolidColorBrush(value->GetType() == Color::typeid ? Colors::Black : (Color)value);
}

Object ^ sgCore::BrushConverter::ConvertBack(Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Object ^parameter, String ^language)
{
	if (dynamic_cast<SolidColorBrush^>(value) != nullptr)
	{
		return safe_cast<SolidColorBrush^>(value)->Color;
	}
	else
	{
		return Colors::Black;
	}

}
