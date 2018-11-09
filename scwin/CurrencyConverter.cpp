#include "pch.h"
#include "CurrencyConverter.h"

using namespace Windows::System;


CurrencyConverter::CurrencyConverter()
{
}

Platform::Object ^ scwin::CurrencyConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	// Determine the current user's default currency.
	auto userCurrency = Windows::System::UserProfile::GlobalizationPreferences::Currencies->GetAt(0);
	// Currency formatter using the current user's preference settings for number formatting.
	auto userCurrencyFormat = ref new Windows::Globalization::NumberFormatting::CurrencyFormatter(userCurrency);
	auto number = dynamic_cast<IBox<float>^>(value);
	if (number == nullptr)
	{
		return userCurrencyFormat->Format(0.);
	}
	return userCurrencyFormat->Format(number->Value);
}

Platform::Object ^ scwin::CurrencyConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	String^ val = (String^)value;
	if (StringHelpers::IsNullOrEmpty(val))
	{
		val = "0.";
	}
	return std::stof(STRCONV(val->Data()), nullptr);
}
