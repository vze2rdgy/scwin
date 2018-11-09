#include "pch.h"
#include "DataStateEnableConverter.h"


DataStateEnableConverter::DataStateEnableConverter()
{
}

Platform::Object ^ scwin::DataStateEnableConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	IBox<DataState>^ prm ;
	if (parameter == nullptr || nullptr == (prm = dynamic_cast<IBox<DataState>^>(parameter)))
	{
		return false;
	}
	IDTOBase^ data = dynamic_cast<IDTOBase^>(value);
	if (data == nullptr)
		return false;
	return data->ObjectState == prm->Value;
}

Platform::Object ^ scwin::DataStateEnableConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw Exception::CreateException(E_FAIL, "DataStateEnableConverter: Two way binding not supported.");
}
