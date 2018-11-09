//
// CustomerAdminView.xaml.cpp
// Implementation of the CustomerAdminView class
//

#include "pch.h"
#include "CustomerAdminView.xaml.h"
#include "AddEditLocation.xaml.h"
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

CustomerAdminView::CustomerAdminView()
{
	InitializeComponent();
}

void scwin::CustomerAdminView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	ViewModel = (CustomerViewModel^)e->Parameter;
	assert(ViewModel != nullptr);
	Customer = ViewModel->Current;
	assert(Customer != nullptr);
	scMainViewModel::Instance->BreadCrumbs->Append(Customer->Organization->ObjectState == DataState::Added ? "New Customer" : Customer->Organization->Name);
}

void scwin::CustomerAdminView::OnNavigatingFrom(NavigatingCancelEventArgs ^ e)
{
	if (_bIsEditing)
	{
		e->Cancel = true;
		return;
	}
	scMainViewModel::Instance->BreadCrumbs->RemoveAtEnd();
	Customer = nullptr;
	ViewModel = nullptr;
}


void scwin::CustomerAdminView::btnEditCustomer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "DisplayCustomerOrgEditPanelState");
	svCustomerDetails->IsPaneOpen = true;
	PaneTitle = ViewModel->Current->Organization->Name;
	_activePanel = 0;
}


void scwin::CustomerAdminView::btnAddLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "DisplayLocationEditPanelState");
	ctlAddEditLocation->Location =
		ref new CostCenterDtosGroupedBindable();
	ctlAddEditLocation->Location->Org = ViewModel->Current;
	svCustomerDetails->IsPaneOpen = true;
	PaneTitle = "New Location";
	_activePanel = 1;
}


void scwin::CustomerAdminView::btnAddContact_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "DisplayContactEditPanelState");
	svCustomerDetails->IsPaneOpen = true;
	PaneTitle = "New Contact";
	auto dc = (CostCenterDtosGroupedBindable^)((Button^)sender)->DataContext;
	auto contactInfo = ref new CustomerContactKeyValuePair(0, ref new CustomerContact(), dc);
	ctlAddEditCustomerContact->ContactInfo = contactInfo;
	_activePanel = 2;
}


void scwin::CustomerAdminView::btnEditLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "DisplayLocationEditPanelState");
	auto dc = (CostCenterDtosGroupedBindable^)((Button^)sender)->DataContext;
	ctlAddEditLocation->Location = dc;
	ctlAddEditLocation->Location->Org = ViewModel->Current;
	svCustomerDetails->IsPaneOpen = true;
	PaneTitle = dc->CostCenter->Name;
	_activePanel = 1;
}


void scwin::CustomerAdminView::btnEditContact_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	GOTOVIEWSTATE(this, "DisplayContactEditPanelState");
	svCustomerDetails->IsPaneOpen = true;
	auto dc = (CustomerContactKeyValuePair^)((Button^)sender)->DataContext;
	PaneTitle = dc->Value->FullName;
	ctlAddEditCustomerContact->ContactInfo = dc;
	_activePanel = 2;
}


void scwin::CustomerAdminView::btnPanelEdit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	bool isAdded = false;
	switch (_activePanel)
	{
	case 0:
		ViewModel->Current->SaveOrganization();
		break;
	case 1:
		isAdded = ctlAddEditLocation->Location->CostCenter->ObjectState == DataState::Added;
		if (ctlAddEditLocation->Location->Save())
		{
			if (isAdded)
				ViewModel->Current->CostCenters->Append(ctlAddEditLocation->Location);
			svCustomerDetails->IsPaneOpen = false;
		}
		break;
	case 2:
		isAdded = ctlAddEditCustomerContact->ContactInfo->Value->ObjectState == DataState::Added;
		if (ctlAddEditCustomerContact->ContactInfo->Save())
		{
			if (isAdded)
				ctlAddEditCustomerContact->ContactInfo->Costcenter->Contacts->Append(ctlAddEditCustomerContact->ContactInfo);
			svCustomerDetails->IsPaneOpen = false;
		}
		break;
	}
}


void scwin::CustomerAdminView::btnPanelCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	switch (_activePanel)
	{
	case 0:
		svCustomerDetails->IsPaneOpen = false;
		break;
	case 1:
		if (ctlAddEditLocation->Location->Cancel())
			svCustomerDetails->IsPaneOpen = false;
		break;
	case 2:
	{
		if (ctlAddEditCustomerContact->ContactInfo->Cancel())
			svCustomerDetails->IsPaneOpen = false;
	}
	break;
	}
}


void scwin::CustomerAdminView::svCustomerDetails_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args)
{
	_activePanel = -1;

}


void scwin::CustomerAdminView::svCustomerDetails_PaneOpening(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args)
{

}


DEFINERAISEPROPCHANGEDSELF(CustomerAdminView);