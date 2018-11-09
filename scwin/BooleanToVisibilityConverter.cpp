#include "pch.h"
#include "BooleanToVisibilityConverter.h"

namespace scwin
{

	BooleanToVisibilityConverter::BooleanToVisibilityConverter()
	{
	}

	Object^ BooleanToVisibilityConverter::Convert(Object ^value, TypeName targetType, Object ^parameter, String ^language)
	{
		IBox<Boolean>^ visibility = dynamic_cast<IBox<Boolean>^>(value);
		if (visibility != nullptr && visibility->Value != IsInverse)
		{
			return Visibility::Visible;
		}
		else
		{
			return Visibility::Collapsed;
		}
	}

	Object ^ BooleanToVisibilityConverter::ConvertBack(Object ^value, TypeName targetType, Object ^parameter, String ^language)
	{
		IBox<Visibility>^ visibility = dynamic_cast<IBox<Visibility>^>(value);
		if (value != nullptr)
		{
			if (visibility->Value == Visibility::Visible)
			{
				return IsInverse ? true : false;
			}
			else
			{
				return !IsInverse ? true : false;
			}
		}
		return IsInverse ? true : false;
	}

}