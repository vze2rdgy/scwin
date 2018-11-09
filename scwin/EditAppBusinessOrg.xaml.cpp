//
// EditAppBusinessOrg.xaml.cpp
// Implementation of the EditAppBusinessOrg class
//

#include "pch.h"
#include "EditAppBusinessOrg.xaml.h"


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

EditAppBusinessOrg::EditAppBusinessOrg()
{
	InitializeComponent();
}

AppBusinessOrgFacade^ EditAppBusinessOrg::ViewModel::get()
{
	return _vm;
}

void EditAppBusinessOrg::ViewModel::set(AppBusinessOrgFacade^ val)
{
	if (_vm != val)
	{
		_vm = val;
		RaisePropertyChanged(PROPERTYNAME(ViewModel));
	}
}

DEFINERAISEPROPCHANGEDSELF(EditAppBusinessOrg);