//
// FamilySetupLookup.xaml.cpp
// Implementation of the FamilySetupLookup class
//

#include "pch.h"
#include "FamilySetupLookup.xaml.h"

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

// The Content Dialog item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

scwin::FamilySetupLookup::FamilySetupLookup(AttributeViewModelFacade^ attribfacade)
{
	this->attribfacade = attribfacade;
	InitializeComponent();

}



void scwin::FamilySetupLookup::btnCloseWindow_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Hide();
}


void scwin::FamilySetupLookup::btnAccept_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	// associate current selected family with the selected product

	if (ProductFamilies->SelectedFamily == nullptr || ProductFamilies->SelectedFamily->ObjectState != DataState::Unmodified)
	{
		auto dlg = ref new MessageDialog("Currently selected family item is not saved. Click save button to save the family or cancel button to cancel changes.", "Accept Failed");
		dlg->Commands->Append(ref new UICommand("Close"));
		dlg->ShowAsync();
		return;
	}

	String^ prodId = attribfacade->SelectedProduct->Id;
	String^ familyId = ProductFamilies->SelectedFamily->Id;

	// associate 
	auto status = scLocalDataService::Instance()->LinkProductToFamily(familyId, prodId);
	if (HASFAILED(status))
	{
		auto dlg = ref new MessageDialog("Failed to link this family with currently selected product. Please retry.", "Accept Failed");
		dlg->Commands->Append(ref new UICommand("Close"));
		dlg->ShowAsync();
	}
	else
	{
		attribfacade->Family = ProductFamilies->SelectedFamily;
		Hide();
	}

}

