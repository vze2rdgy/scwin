//
// AddressEditControl.xaml.cpp
// Implementation of the AddressEditControl class
//

#include "pch.h"
#include "AddressEditControl.xaml.h"

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

DependencyProperty^ AddressEditControl::headerProperty = nullptr;
DependencyProperty^ AddressEditControl::addressProperty = nullptr;
DependencyProperty^ AddressEditControl::showListOfAddressesProperty = nullptr;
DependencyProperty^ AddressEditControl::canEditAddressProperty = nullptr;
DependencyProperty^ AddressEditControl::addressListProperty = nullptr;

void scwin::AddressEditControl::Register()
{
	if (headerProperty == nullptr)
		headerProperty = DependencyProperty::Register(
			"Header",
			String::typeid,
			AddressEditControl::typeid,
			ref new PropertyMetadata("Address")
		);

	if (addressProperty == nullptr)
		addressProperty = DependencyProperty::Register(
			"Address",
			Address::typeid,
			AddressEditControl::typeid,
			ref new PropertyMetadata(ref new scwin::Address())
		);

	if (addressListProperty == nullptr)
		addressListProperty = DependencyProperty::Register(
			"AddressList",
			IObservableVector<scwin::Address^>::typeid,
			AddressEditControl::typeid,
			ref new PropertyMetadata(ref new Vector<scwin::Address^>())
		);

	if (showListOfAddressesProperty == nullptr)
		showListOfAddressesProperty = DependencyProperty::Register(
			"ShowListOfAddresses",
			bool::typeid,
			AddressEditControl::typeid,
			ref new PropertyMetadata(true)
		);

	if (canEditAddressProperty == nullptr)
		canEditAddressProperty = DependencyProperty::Register(
			"CanEditAddress",
			bool::typeid,
			AddressEditControl::typeid,
			ref new PropertyMetadata(true)
		);

}

AddressEditControl::AddressEditControl()
{
	InitializeComponent();
}

void scwin::AddressEditControl::OnAddressPropertyChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ eargs)
{
	auto addr = eargs->NewValue;
	if (addr == nullptr)
	{
	}
}

String^ AddressEditControl::Header::get()
{
	return (String^)GetValue(headerProperty);
}

void AddressEditControl::Header::set(String^ val)
{
	SetValue(headerProperty, val);
}

scwin::Address^ AddressEditControl::Address::get()
{
	return (scwin::Address^)GetValue(addressProperty);
}

void AddressEditControl::Address::set(scwin::Address^ val)
{
	SetValue(addressProperty, val);
}

IObservableVector<scwin::Address^>^ AddressEditControl::AddressList::get()
{
	return (IObservableVector<scwin::Address^>^)GetValue(addressListProperty);
}

void AddressEditControl::AddressList::set(IObservableVector<scwin::Address^>^ val)
{
	SetValue(addressListProperty, val);
}

bool AddressEditControl::ShowListOfAddresses::get()
{
	auto boxed = dynamic_cast<IBox<bool>^>(GetValue(showListOfAddressesProperty));
	if (boxed != nullptr)
		return boxed->Value;
	return true;
}

void AddressEditControl::ShowListOfAddresses::set(bool val)
{
	SetValue(showListOfAddressesProperty, val);
}

bool AddressEditControl::CanEditAddress::get()
{
	auto boxed = dynamic_cast<IBox<bool>^>(GetValue(canEditAddressProperty));
	if (boxed != nullptr)
		return boxed->Value;
	return true;
}

void AddressEditControl::CanEditAddress::set(bool val)
{
	SetValue(canEditAddressProperty, val);
}

void scwin::AddressEditControl::cmbAddressList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (e->AddedItems->Size > 0)
	{
		this->Address = (scwin::Address^)e->AddedItems->GetAt(0);
	}
}
