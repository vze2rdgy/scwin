//
// CustomerViewControl.xaml.cpp
// Implementation of the CustomerViewControl class
//

#include "pch.h"
#include "CustomerViewControl.xaml.h"

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

DependencyProperty^ CustomerViewControl::customerProperty = nullptr;
DependencyProperty^ CustomerViewControl::costCenterProperty = nullptr;
DependencyProperty^ CustomerViewControl::contactProperty = nullptr;
DependencyProperty^ CustomerViewControl::billingAddressProperty = nullptr;
DependencyProperty^ CustomerViewControl::shippingAddressProperty = nullptr;

CustomerViewControl::CustomerViewControl()
{
	InitializeComponent();
}

void scwin::CustomerViewControl::Register()
{
	if (customerProperty == nullptr)
		customerProperty = DependencyProperty::Register("Customer", CustomerOrg::typeid, CustomerViewControl::typeid, ref new PropertyMetadata(nullptr));
	if (costCenterProperty == nullptr)
		costCenterProperty = DependencyProperty::Register("CostCenter", COrgCostCenter::typeid, CustomerViewControl::typeid, ref new PropertyMetadata(nullptr));
	if (contactProperty == nullptr)
		contactProperty = DependencyProperty::Register("Contact", CustomerContact::typeid, CustomerViewControl::typeid, ref new PropertyMetadata(nullptr));
	if (billingAddressProperty == nullptr)
		billingAddressProperty = DependencyProperty::Register("BillingAddress", Address::typeid, CustomerViewControl::typeid, ref new PropertyMetadata(nullptr));
	if (shippingAddressProperty == nullptr)
		shippingAddressProperty = DependencyProperty::Register("ShippingAddress", Address::typeid, CustomerViewControl::typeid, ref new PropertyMetadata(nullptr));
}
