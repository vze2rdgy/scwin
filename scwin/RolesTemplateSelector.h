#pragma once

#include "AdminViewModel.h"

namespace scwin
{

	public ref class RolesTemplateSelector sealed : DataTemplateSelector
	{
	public:
		RolesTemplateSelector();

		property bool IsSelected;

		virtual DataTemplate^ SelectTemplateCore(Object^ item, DependencyObject ^container) override;

		property DataTemplate^ ViewTemplate;

		property DataTemplate^ EditTemplate;

		property RoleViewModel^ ViewModel;
	};

}