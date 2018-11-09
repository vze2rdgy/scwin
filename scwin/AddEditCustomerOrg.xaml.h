//
// AddEditCustomerOrg.xaml.h
// Declaration of the AddEditCustomerOrg class
//

#pragma once

#include "AddEditCustomerOrg.g.h"
#include "CustomerDtosGroupedBindable.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AddEditCustomerOrg sealed : public INotifyPropertyChanged
	{
		CustomerDtosGroupedBindable^ _custorg;

	public:
		AddEditCustomerOrg();

		property CustomerDtosGroupedBindable^ CustomerOrg
		{
			CustomerDtosGroupedBindable^ get();
			void set(CustomerDtosGroupedBindable^ val);
		}

		property String^ Title;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;

	};
}
