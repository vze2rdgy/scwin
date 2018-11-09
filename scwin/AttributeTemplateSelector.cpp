#include "pch.h"
#include "AttributeTemplateSelector.h"


AttributeTemplateSelector::AttributeTemplateSelector(ResourceDictionary^ resources) :
	controlResources(resources)
{
}

DataTemplate ^ scwin::AttributeTemplateSelector::SelectTemplateCore(Object ^ item)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

DataTemplate ^ scwin::AttributeTemplateSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
