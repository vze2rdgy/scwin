#include "pch.h"
#include "DiscountTypeToTextConverter.h"


DiscountTypeToTextConverter::DiscountTypeToTextConverter()
{
}

Platform::Object ^ scwin::DiscountTypeToTextConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	IBox<DiscountTypes>^ boxed = dynamic_cast<IBox<DiscountTypes>^>(value);
	if (boxed == nullptr)
		return "";
	if (boxed->Value == DiscountTypes::Amount)
		return "#";
	else
		return "%";
}

Platform::Object ^ scwin::DiscountTypeToTextConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	return nullptr;
}

