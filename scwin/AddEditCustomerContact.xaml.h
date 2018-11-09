//
// AddEditCustomerContact.xaml.h
// Declaration of the AddEditCustomerContact class
//

#pragma once

#include "AddEditCustomerContact.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AddEditCustomerContact sealed : public INotifyPropertyChanged
	{
		CustomerContactKeyValuePair^ _contactInfo;

	public:
		AddEditCustomerContact();

		property CustomerContactKeyValuePair^ ContactInfo
		{
			CustomerContactKeyValuePair^ get();
			void set(CustomerContactKeyValuePair^ val);
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;
	};
}
