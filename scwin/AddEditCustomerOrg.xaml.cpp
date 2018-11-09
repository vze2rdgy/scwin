//
// AddEditCustomerOrg.xaml.cpp
// Implementation of the AddEditCustomerOrg class
//

#include "pch.h"
#include "AddEditCustomerOrg.xaml.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

AddEditCustomerOrg::AddEditCustomerOrg()
{
	InitializeComponent();
}

CustomerDtosGroupedBindable^ AddEditCustomerOrg::CustomerOrg::get()
{
	return _custorg;
}

void AddEditCustomerOrg::CustomerOrg::set(CustomerDtosGroupedBindable^ val)
{
	if (val != _custorg)
	{
		_custorg = val;
		RaisePropertyChanged(PROPERTYNAME(AddEditCustomerOrg));
		if (_custorg->Organization->ObjectState == DataState::Added)
			Title = "New Customer Organization";
		else
			Title = _custorg->Organization->Name;
		RaisePropertyChanged(PROPERTYNAME(Title));
	}
}

DEFINERAISEPROPCHANGEDSELF(AddEditCustomerOrg);