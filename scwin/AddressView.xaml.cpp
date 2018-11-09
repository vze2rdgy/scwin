//
// AddressView.xaml.cpp
// Implementation of the AddressView class
//

#include "pch.h"
#include "AddressView.xaml.h"

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

DependencyProperty^ AddressView::headerProperty = nullptr;
DependencyProperty^ AddressView::addressInstanceProperty = nullptr;

AddressView::AddressView()
{
	InitializeComponent();
}

void scwin::AddressView::Register()
{
	if (headerProperty == nullptr)
		headerProperty = DependencyProperty::Register("Header", String::typeid, AddressView::typeid, ref new PropertyMetadata("AddressHeader"));
	if (addressInstanceProperty == nullptr)
		addressInstanceProperty = DependencyProperty::Register("AddressInstance", Address::typeid, AddressView::typeid, ref new PropertyMetadata(nullptr));
}
