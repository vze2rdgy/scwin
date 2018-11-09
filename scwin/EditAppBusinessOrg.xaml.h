//
// EditAppBusinessOrg.xaml.h
// Declaration of the EditAppBusinessOrg class
//

#pragma once

#include "EditAppBusinessOrg.g.h"
#include "AppBusinessOrg.h"
#include "SettingsViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class EditAppBusinessOrg sealed : public INotifyPropertyChanged
	{
		AppBusinessOrgFacade^ _vm;

	public:
		EditAppBusinessOrg();

		property AppBusinessOrgFacade^ ViewModel
		{
			AppBusinessOrgFacade^ get();
			void set(AppBusinessOrgFacade^ val);
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;

	};
}
