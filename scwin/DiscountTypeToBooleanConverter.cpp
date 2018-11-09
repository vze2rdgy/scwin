#include "pch.h"
#include "DiscountTypeToBooleanConverter.h"


DiscountTypeToBooleanConverter::DiscountTypeToBooleanConverter()
{
}


Platform::Object ^ scwin::DiscountTypeToBooleanConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	IBox<DiscountTypes>^ boxed = dynamic_cast<IBox<DiscountTypes>^>(value);
	if (boxed == nullptr)
		return nullptr;
	if (boxed->Value == DiscountTypes::Amount)
		return true;
	else
		return false;
}

Platform::Object ^ scwin::DiscountTypeToBooleanConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	IBox<Boolean>^ boxed = dynamic_cast<IBox<Boolean>^>(value);
	if (boxed == nullptr) return DiscountTypes::Unknown;
	if (boxed->Value)
		return DiscountTypes::Amount;
	else
		return DiscountTypes::Percent;
}
