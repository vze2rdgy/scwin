//
// EditAppBusinessMoreDetails.xaml.h
// Declaration of the EditAppBusinessMoreDetails class
//

#pragma once

#include "EditAppBusinessMoreDetails.g.h"
#include "SettingsViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class EditAppBusinessMoreDetails sealed : public INotifyPropertyChanged
	{
		AppBusinessOrgFacade^ _vm;

	public:
		EditAppBusinessMoreDetails();

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
