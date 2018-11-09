//
// Administration.xaml.cpp
// Implementation of the Administration class
//

#include "pch.h"
#include "Administration.xaml.h"
#include "SharedViewModels.h"

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

// Roles and Users will have different templates based on view or edit mode
// https://docs.microsoft.com/en-us/uwp/api/Windows.UI.Xaml.Controls.DataTemplateSelector
// https://docs.microsoft.com/en-us/windows/uwp/data-binding/data-binding-in-depth
//https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.itemscontrol#Windows_UI_Xaml_Controls_ItemsControl_ItemTemplateSelector

Administration::Administration()
{
	this->ViewModel = ref new AdminViewModel(scMainViewModel::Instance);
	InitializeComponent();
	this->DataContext = this->ViewModel;
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::Administration::OnLoaded);
}

Administration::~Administration()
{
	ViewModel->RolesViewModel->PropertyChanged -= rolevmChangedtoken;
	ColorMasking::UnregisterDependencyProperties();
}

void scwin::Administration::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);
	// select pivot index
	if (e->Parameter )
	{
		auto vm = SharedViewModels::SetGetViewModel<ProductViewModel>(SharedViewModelIds::ProductViewModel);
		Map <String^, Object^>^ parameters = safe_cast<Map <String^, Object^>^>(e->Parameter);
		auto pivotIndex = (int)parameters->Lookup(L"PivotIndex");
		auto pvm = (ProductViewModel^)parameters->Lookup(L"ViewModel");
		AdminPivot->SelectedIndex = pivotIndex;
		OutputDebugString(AdminPivot->SelectedItem->GetType()->FullName->Data());
		auto pvt = safe_cast<PivotItem^>(AdminPivot->SelectedItem);
		if (pvt)
		{
			pvt->DataContext = pvm;
		}
	}
}

void scwin::Administration::lvAdminRoles_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{

}


void scwin::Administration::chkEditRole_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	CheckBox^ chk = static_cast<CheckBox^>(sender);
	//bool result;
	if (chk->IsChecked)
	{
		//result = VisualStateManager::GoToState(lvAdminRoles, L"HidingShowingEditControlsForUserRoles", true);
	}
	else
	{
		//result = VisualStateManager::GoToState(lvAdminRoles, L"ShowingHidingEditControlsForUserRoles", true);
	}
	//OutputDebugString(result ? L"Transitioned successfully\n" : L"Failed to transition\n");
}


void scwin::Administration::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
}


void scwin::Administration::AdminPivot_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	//if (AdminPivot->SelectedIndex == ADMINPIVOTPRODUCTSINDEX)
	//{
	//	auto pvt = dynamic_cast<PivotItem^>(AdminPivot->SelectedItem);
	//	if (pvt)
	//	{
	//		MultiViewControl^ page = dynamic_cast<MultiViewControl^>(pvt->Content);
	//	}
	//}
}
