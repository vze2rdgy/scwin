//
// AddEditCustomerContact.xaml.cpp
// Implementation of the AddEditCustomerContact class
//

#include "pch.h"
#include "AddEditCustomerContact.xaml.h"

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

AddEditCustomerContact::AddEditCustomerContact()
{
	InitializeComponent();
}

CustomerContactKeyValuePair^ AddEditCustomerContact::ContactInfo::get()
{
	return _contactInfo;
}

void AddEditCustomerContact::ContactInfo::set(CustomerContactKeyValuePair^ val)
{
	if (_contactInfo != val)
	{
		_contactInfo = val;
		RaisePropertyChanged(PROPERTYNAME(ContactInfo));
	}
}

DEFINERAISEPROPCHANGEDSELF(AddEditCustomerContact);