#include "pch.h"
#include "StatusMessageTemplateSelector.h"


StatusMessageTemplateSelector::StatusMessageTemplateSelector(ResourceDictionary^ controlResources) : 
	controlResources(controlResources)
{
}


DataTemplate ^ scwin::StatusMessageTemplateSelector::SelectTemplateCore(Object ^ data)
{
	return SelectTemplateCore(data, nullptr);
}

DataTemplate ^ scwin::StatusMessageTemplateSelector::SelectTemplateCore(Object ^ data, DependencyObject ^ container)
{
	if (data == nullptr)
		return nullptr;

	auto msginfo = dynamic_cast<StatusMessage^>(data);
	auto res = controlResources.Resolve<ResourceDictionary>();
	if (msginfo != nullptr)
	{
		switch (msginfo->Type)
		{
		case StatusMessageTypes::Error:
			return (DataTemplate^)res->Lookup("StatusMessageItemTemplateForError");
			break;
		case StatusMessageTypes::Information:
			return (DataTemplate^)res->Lookup("StatusMessageItemTemplateForInfo");
			break;
		case StatusMessageTypes::Warning:
			return (DataTemplate^)res->Lookup("StatusMessageItemTemplateForWarning");
			break;
		}
	}
	return (DataTemplate^)res->Lookup("StatusMessageItemTemplateForInfo");
}
