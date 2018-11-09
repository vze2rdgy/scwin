#include "pch.h"
#include "StringToDataTemplateConverter.h"


StringToDataTemplateConverter::StringToDataTemplateConverter(ResourceDictionary^ resources)
{
	Resources = resources;
}

Platform::Object ^ scwin::StringToDataTemplateConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	String^ templateName = dynamic_cast<String^>(value);
	if (templateName == nullptr)
		return nullptr;
	DataTemplate^ dtempl = (DataTemplate^)Resources->Lookup(templateName);
	return dtempl;
}

Platform::Object ^ scwin::StringToDataTemplateConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
}
