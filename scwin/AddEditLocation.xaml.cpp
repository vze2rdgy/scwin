//
// AddEditLocation.xaml.cpp
// Implementation of the AddEditLocation class
//

#include "pch.h"
#include "AddEditLocation.xaml.h"
#include "XamlHelper.h"

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

AddEditLocation::AddEditLocation()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::AddEditLocation::OnLoaded);
}

scwin::CostCenterDtosGroupedBindable^ AddEditLocation::Location::get()
{
	return _location;
}

void AddEditLocation::Location::set(CostCenterDtosGroupedBindable^ val)
{
	if (val != _location)
	{
		_location = val;
		if (_location->CostCenter->ObjectState == DataState::Added)
		{
			Title = "New Location";
		}
		else
		{
			Title = _location->CostCenter->Name;
		}
		RaisePropertyChanged(PROPERTYNAME(Location));
		RaisePropertyChanged(PROPERTYNAME(Title));
	}
}


DEFINERAISEPROPCHANGEDSELF(AddEditLocation);

void scwin::AddEditLocation::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	IVector<ExpandPanel^>^ children;
	if (XamlHelper::GetChildrenOfType<ExpandPanel>(this, children))
	{
		for (auto ep : to_vector(children))
			ep->IsExpanded = false;
	}
}


void scwin::AddEditLocation::cmbShippingSameAs_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto selection = (String^)cmbBillingSameAs->SelectedValue;
	SettleSameAsAddress(SHIPPING, selection);
}


void scwin::AddEditLocation::cmbBillingSameAs_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto selection = (String^)cmbBillingSameAs->SelectedValue;
	SettleSameAsAddress(BILLING, selection);
}

void scwin::AddEditLocation::SettleSameAsAddress(String^ addrkey, String^ selection)
{
	if (!StringHelpers::IsNullOrEmpty(selection))
	{
		if (addrkey == BILLING)
		{
			Location->BillingAddress = selection == MAILING ? Location->MailingAddress : Location->ShippingAddress;
		}
		else if (addrkey == SHIPPING)
		{
			Location->ShippingAddress = selection == MAILING ? Location->MailingAddress : Location->BillingAddress;
		}
	}
	else
	{
		Location->BillingAddress = ref new AddressKeyValuePair(addrkey, ref new Address(), Location->CostCenter->Id);
	}
}
