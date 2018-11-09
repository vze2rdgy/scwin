#include "pch.h"
#include "SettingsViewModel.h"
#include "SettingsViewSidePanelContentTemplSelector.h"


SettingsViewSidePanelContentTemplSelector::SettingsViewSidePanelContentTemplSelector(ResourceDictionary^ resources, SettingsViewModel^ viewModel) 
	: _resources(resources), _viewModel(viewModel)
{
}

DataTemplate ^ scwin::SettingsViewSidePanelContentTemplSelector::SelectTemplateCore(Object ^ item)
{
	return SelectTemplateCore(item, nullptr);
}

DataTemplate ^ scwin::SettingsViewSidePanelContentTemplSelector::SelectTemplateCore(Object ^ item, DependencyObject ^ container)
{
	// more conditions based on viewmodel
	return (DataTemplate^)_resources->Lookup(_viewModel->ActiveDataTemplateName);
}

