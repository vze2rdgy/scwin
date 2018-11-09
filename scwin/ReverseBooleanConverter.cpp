#include "pch.h"
#include "ReverseBooleanConverter.h"

using namespace scwin;

ReverseBooleanConverter::ReverseBooleanConverter()
{

}

Platform::Object ^ ReverseBooleanConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	IBox<Boolean>^ bvalue = dynamic_cast<IBox<Boolean>^>(value);
	if (bvalue == nullptr)
		return true;
	return !bvalue->Value;
}

Platform::Object ^ ReverseBooleanConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	IBox<Boolean>^ bvalue = dynamic_cast<IBox<Boolean>^>(value);
	if (bvalue == nullptr)
		return true;
	return !bvalue->Value;
}
