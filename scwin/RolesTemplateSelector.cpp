#include "pch.h"
#include "RolesTemplateSelector.h"

using namespace scwin;

RolesTemplateSelector::RolesTemplateSelector()
{
}

DataTemplate ^ scwin::RolesTemplateSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	if (!IsSelected)
		return this->ViewTemplate;
	else
		return this->EditTemplate;
}
