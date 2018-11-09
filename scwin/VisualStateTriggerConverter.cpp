#include "pch.h"
#include "VisualStateTriggerConverter.h"


VisualStateTriggerConverter::VisualStateTriggerConverter()
{
}

Platform::Object ^ scwin::VisualStateTriggerConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	String^ prm = dynamic_cast<String^>(parameter);
	auto trigger = dynamic_cast<IBox<VisualStateTriggers>^>(value);
	if (prm != nullptr)
	{
		if (prm == "NoDataState")
		{
			return trigger->Value == VisualStateTriggers::NoDataState;
		}
		else if (prm == "ReadState")
		{
			return trigger->Value == VisualStateTriggers::ReadState;
		}
		else if (prm == "WriteState")
		{
			return trigger->Value == VisualStateTriggers::WriteState;
		}
		else
		{
			return false;
		}
	}
	else
	{
		throw Exception::CreateException(E_FAIL, "Converter Parameter not provided. The values must be one of NoDataState, ReadState, WriteState");
	}
}

Platform::Object ^ scwin::VisualStateTriggerConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	return nullptr;
}
