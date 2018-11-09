//
// ChoooseACustomer.xaml.cpp
// Implementation of the ChoooseACustomer class
//

#include "pch.h"
#include "ChoooseACustomer.xaml.h"
#include "XamlHelper.h"
#include "cpplinq.hpp"

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

//DependencyProperty^ ChoooseACustomer::salesviewmodelProperty = nullptr;
DependencyProperty^ ChoooseACustomer::customerviewmodelProperty = nullptr;
DependencyProperty^ ChoooseACustomer::customerProperty = nullptr;
DependencyProperty^ ChoooseACustomer::costcenterProperty = nullptr;
DependencyProperty^ ChoooseACustomer::contactProperty = nullptr;


void scwin::ChoooseACustomer::Register()
{
	//if (salesviewmodelProperty == nullptr)
	//{
	//	salesviewmodelProperty = DependencyProperty::Register(
	//		"SalesViewmodel",
	//		SaleOrderViewModel::typeid,
	//		ChoooseACustomer::typeid,
	//		ref new PropertyMetadata(nullptr)
	//	);
	//}
	if (customerviewmodelProperty == nullptr)
	{
		customerviewmodelProperty = DependencyProperty::Register(
			"CustomerViewmodel",
			CustomerViewModel::typeid,
			ChoooseACustomer::typeid,
			ref new PropertyMetadata(nullptr)
		);
	}
	if (customerProperty == nullptr)
	{
		customerProperty = DependencyProperty::Register(
			"Customer",
			CustomerOrg::typeid,
			ChoooseACustomer::typeid,
			ref new PropertyMetadata(nullptr)
		);
	}
	if (costcenterProperty == nullptr)
	{
		costcenterProperty= DependencyProperty::Register(
			"CostCenter",
			COrgCostCenter::typeid,
			ChoooseACustomer::typeid,
			ref new PropertyMetadata(nullptr)
		);
	}
	if (contactProperty == nullptr)
	{
		contactProperty = DependencyProperty::Register(
			"Contact",
			CustomerContact::typeid,
			ChoooseACustomer::typeid,
			ref new PropertyMetadata(nullptr)
		);
	}
}

ChoooseACustomer::ChoooseACustomer()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ChoooseACustomer::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ChoooseACustomer::OnUnloaded);
}



bool scwin::ChoooseACustomer::HasPatternMatchedOrg(CustomerDtosGroupedBindable^ org, String ^ pattern)
{
	OutputDebugString(L"Organization: "); OutputDebugString(org->Organization->Name->Data()); OutputDebugString(L"\n");

	if (StringHelpers::Match(org->Organization->Name, pattern))
		return true;

	auto defCC = org->DefaultCostCenter;
	if (StringHelpers::Match(defCC->CostCenter->Name, pattern))
		return true;


	for (auto c : defCC->Contacts)
	{
		OutputDebugString(c->Value->FirstName->Data()); OutputDebugString(L", "); OutputDebugString(c->Value->FirstName->Data()); OutputDebugString(L"\n");
		if (StringHelpers::Match(c->Value->FirstName, pattern) || StringHelpers::Match(c->Value->LastName, pattern))
			return true;
	}


	for (auto cc : org->CostCenters)
	{
		OutputDebugString(L"CostCenter: ");OutputDebugString(cc->CostCenter->Name->Data()); OutputDebugString(L"\n");

		if (StringHelpers::Match(cc->CostCenter->Name, pattern))
			return true;


		for (auto c : cc->Contacts)
		{
			OutputDebugString(c->Value->FirstName->Data()); OutputDebugString(L", "); OutputDebugString(c->Value->FirstName->Data()); OutputDebugString(L"\n");
			if (StringHelpers::Match(c->Value->FirstName, pattern) || StringHelpers::Match(c->Value->LastName, pattern))
				return true;
		}
	}
	return false;
}

void scwin::ChoooseACustomer::DisplayOnlyMatchingOrgs(String ^ pattern)
{
	auto listSize = CustomerViewmodel->Organizations->Size;
	if (StringHelpers::IsNullOrEmpty(pattern) && listSize != lVisibleOrgsCount)
	{
		// make all visible
		for (auto org : CustomerViewmodel->Organizations)
		{
			auto container = gvOrgs->ContainerFromItem(org);
			if (container != nullptr)
			{
				safe_cast<GridViewItem^>(container)->Visibility = Windows::UI::Xaml::Visibility::Visible;
			}
		}
		return;
	}

	long counter = 0;
	for (unsigned long l = 0; l < CustomerViewmodel->Organizations->Size; l++)
	{
		auto org = CustomerViewmodel->Organizations->GetAt(l);
		auto visibility = HasPatternMatchedOrg(org, pattern) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
		auto container = gvOrgs->ContainerFromItem(org);
		if (container != nullptr)
		{
			safe_cast<GridViewItem^>(container)->Visibility = visibility;
			if (visibility == Windows::UI::Xaml::Visibility::Visible)
				counter++;
		}
	}

	lVisibleOrgsCount = counter;
}

void scwin::ChoooseACustomer::txtFindCustomer_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	// dispatch it
	Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]() -> void
	{
		DisplayOnlyMatchingOrgs(sender->Text);
	}));
}


void scwin::ChoooseACustomer::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	lVisibleOrgsCount = CustomerViewmodel->Organizations->Size;
	// update selections of costcenter and contact for radio button
	// since customerviewmodel is a new instance for each salesorderview.xaml instance,
	// previously set values will stay once this view selects them. However when an order which is already saved before
	// is loaded, customerviewmodel properties for the radiobuttons will have to be updated for first time.
	// this can be done in salesorderview.xaml itself.
}


void scwin::ChoooseACustomer::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	
}


void scwin::ChoooseACustomer::rdContact_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	using namespace cpplinq;
	auto dc = (CustomerContactKeyValuePair^)((RadioButton^)sender)->DataContext;
	if (dc == nullptr) return;
	Contact = dc->Value;
	CostCenter = nullptr;
	// locate the contact in the customerviewmodel
	for (auto org : CustomerViewmodel->Organizations)
	{
		for (auto c : org->DefaultCostCenter->Contacts)
		{
			if (c->Value == dc->Value)
			{
				org->DefaultCostCenter->IsCostCenterSelectedForSalesOrder = true;
				CostCenter = org->DefaultCostCenter->CostCenter;
				Customer = org->Organization;
				Contact = c->Value;
				return;
			}
		}
		for (auto cc : org->CostCenters)
		{
			for (auto c : cc->Contacts)
			{
				if (c->Value == dc->Value)
				{
					cc->IsCostCenterSelectedForSalesOrder = true;
					CostCenter = cc->CostCenter;
					Customer = org->Organization;
					Contact = c->Value;
					return;
				}
			}
		}
	}
}

void scwin::ChoooseACustomer::rdCostCenter_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto dc = (CostCenterDtosGroupedBindable^)((RadioButton^)sender)->DataContext;
	if (dc == nullptr) return;
	CostCenter = dc->CostCenter;
	Contact = nullptr;

	CustomerOrg^ selectedOrg = nullptr;
	CustomerContact^ selectedContact = nullptr;

	// disable any contact selection if not already of this costcenter's contacts
	for (auto org : CustomerViewmodel->Organizations)
	{
		auto defcc = org->DefaultCostCenter;
		if (dc != defcc)
		{
			for (auto c : defcc->Contacts)
			{
				c->IsContactSelectedForSalesOrder = false;
			}
		}
		else
		{
			selectedOrg = org->Organization;
		}
		for (auto cc : org->CostCenters)
		{
			if (dc != cc)
			{
				for (auto c : cc->Contacts)
				{
					c->IsContactSelectedForSalesOrder = false;
				}
			}
			else
			{
				selectedOrg = org->Organization;
			}
		}
	}
	Customer = selectedOrg;
}


void scwin::ChoooseACustomer::btnSelectCustomer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Popup^ pp;
	if (XamlHelper::FindAncestor((FrameworkElement^)sender, &pp))
	{
		pp->IsOpen = false;
	}

}
