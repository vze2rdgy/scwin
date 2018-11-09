#pragma once


namespace scwin
{
	ref class SettingsViewModel;

	public ref class SettingsViewSidePanelContentTemplSelector sealed : DataTemplateSelector
	{
		ResourceDictionary^ _resources;
		SettingsViewModel^ _viewModel;

	public:
		SettingsViewSidePanelContentTemplSelector(ResourceDictionary^ resources, SettingsViewModel^ viewModel);

		virtual DataTemplate^ SelectTemplateCore(Object^ item) override;
		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject^ container) override;
	};

}