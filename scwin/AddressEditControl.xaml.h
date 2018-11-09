//
// AddressEditControl.xaml.h
// Declaration of the AddressEditControl class
//

#pragma once

#include "AddressEditControl.g.h"
#include "Address.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AddressEditControl sealed
	{
		static Windows::UI::Xaml::DependencyProperty^ headerProperty;
		static Windows::UI::Xaml::DependencyProperty^ addressProperty;
		static Windows::UI::Xaml::DependencyProperty^ showListOfAddressesProperty;
		static Windows::UI::Xaml::DependencyProperty^ canEditAddressProperty;
		static Windows::UI::Xaml::DependencyProperty^ addressListProperty;

	public:

		static void Register();

		AddressEditControl();

		static property Windows::UI::Xaml::DependencyProperty^ HeaderProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return headerProperty;
			}
		}

		static property Windows::UI::Xaml::DependencyProperty^ AddressProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() 
			{
				return addressProperty;
			}
		}

		static property Windows::UI::Xaml::DependencyProperty^ AddressListProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return addressListProperty;
			}
		}

		static property Windows::UI::Xaml::DependencyProperty^ ShowListOfAddressesProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return showListOfAddressesProperty; }
		}

		static property Windows::UI::Xaml::DependencyProperty^ CanEditAddressProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() { return canEditAddressProperty; }
		}

		property String^ Header
		{
			String^ get();
			void set(String^);
		}

		property scwin::Address^ Address
		{
			scwin::Address^ get();
			void set(scwin::Address^);
		}

		property IObservableVector<scwin::Address^>^ AddressList
		{
			IObservableVector<scwin::Address^>^ get();
			void set(IObservableVector<scwin::Address^>^);
		}

		property bool ShowListOfAddresses
		{
			bool get();
			void set(bool);
		}

		property bool CanEditAddress
		{
			bool get();
			void set(bool);
		}

	private:
		void OnAddressPropertyChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ eargs);
		void cmbAddressList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
	};
}
