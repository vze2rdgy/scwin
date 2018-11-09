//
// CustomerViewControl.xaml.h
// Declaration of the CustomerViewControl class
//

#pragma once

#include "CustomerViewControl.g.h"

#include "CustomerOrg.h"
#include "AddressView.xaml.h"
#include "COrgCostCenter.h"
#include "Address.h"
#include "CustomerContact.h"

using namespace Windows::UI::Xaml;

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CustomerViewControl sealed
	{
		static DependencyProperty^ customerProperty;
		static DependencyProperty^ costCenterProperty;
		static DependencyProperty^ contactProperty;
		static DependencyProperty^ billingAddressProperty;
		static DependencyProperty^ shippingAddressProperty;

	public:
		
		CustomerViewControl();

		static void Register();

		static property DependencyProperty^ CustomerProperty
		{
			DependencyProperty^ get() { return customerProperty; }
		}

		static property DependencyProperty^ CostCenterProperty
		{
			DependencyProperty^ get() { return costCenterProperty; }
		}

		static property DependencyProperty^ ContactProperty
		{
			DependencyProperty^ get() { return contactProperty; }
		}
		static property DependencyProperty^ BillingAddressProperty
		{
			DependencyProperty^ get() { return billingAddressProperty; }
		}
		static property DependencyProperty^ ShippingAddressProperty
		{
			DependencyProperty^ get() { return shippingAddressProperty; }
		}

		property CustomerOrg^ Customer
		{
			CustomerOrg^ get() 
			{ 
				return (CustomerOrg^)GetValue(customerProperty); 
			}

			void set(CustomerOrg^ val)
			{
				SetValue(customerProperty, val);
			}
		}

		property COrgCostCenter^ CostCenter
		{
			COrgCostCenter^ get()
			{
				return (COrgCostCenter^)GetValue(costCenterProperty);
			}

			void set(COrgCostCenter^ val)
			{
				SetValue(costCenterProperty, val);
			}
		}

		property CustomerContact^ Contact
		{
			CustomerContact^ get()
			{
				return (CustomerContact^)GetValue(contactProperty);
			}

			void set(CustomerContact^ val)
			{
				SetValue(contactProperty, val);
			}
		}
		property Address^ BillingAddress
		{
			Address^ get()
			{
				return (Address^)GetValue(billingAddressProperty);
			}

			void set(Address^ val)
			{
				SetValue(billingAddressProperty, val);
			}
		}
		property Address^ ShippingAddress
		{
			Address^ get()
			{
				return (Address^)GetValue(shippingAddressProperty);
			}

			void set(Address^ val)
			{
				SetValue(shippingAddressProperty, val);
			}
		}

	};
}
