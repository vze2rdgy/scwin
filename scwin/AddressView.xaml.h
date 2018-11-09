//
// AddressView.xaml.h
// Declaration of the AddressView class
//

#pragma once

#include "AddressView.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AddressView sealed
	{
		static DependencyProperty^ headerProperty;
		static DependencyProperty^ addressInstanceProperty;

	public:
		AddressView();

		static void Register();

		static property DependencyProperty^ HeaderProperty
		{
			DependencyProperty^ get()
			{
				return headerProperty;
			}
		}

		static property DependencyProperty^ AddressInstanceProperty
		{
			DependencyProperty^ get() { return addressInstanceProperty; }
		}

		property String^ Header
		{
			String^ get() { return (String^)GetValue(headerProperty); }
			void set(String^ val)
			{
				SetValue(headerProperty, val);
			}
		}

		property Address^ AddressInstance
		{
			Address^ get() { return (Address^)GetValue(addressInstanceProperty); }
			void set(Address^ val)
			{
				SetValue(addressInstanceProperty, val);
			}
		}

	};
}
