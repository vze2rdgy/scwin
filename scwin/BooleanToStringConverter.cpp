#include "pch.h"
#include "BooleanToStringConverter.h"


namespace scwin
{

	BooleanToStringConverter::BooleanToStringConverter()
	{

	}

	Object^ BooleanToStringConverter::Convert(Object ^value, TypeName targetType, Object ^parameter, String ^language)
	{
		IBox<Boolean>^ boolstate = dynamic_cast<IBox<Boolean>^>(value);
		if (boolstate != nullptr && boolstate->Value)
		{
			return "Yes";
		}
		else
		{
			return "No";
		}
	}

	Object ^ BooleanToStringConverter::ConvertBack(Object ^value, TypeName targetType, Object ^parameter, String ^language)
	{
		String^ val = (String^)value;
		if (val == "Yes")
			return true;
		else
			return false;
	}

}