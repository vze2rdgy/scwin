//
// ChoooseACustomer.xaml.h
// Declaration of the ChoooseACustomer class
//

#pragma once

#include "ChoooseACustomer.g.h"
#include "SaleOrderViewModel.h"
#include "CustomerViewModel.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ChoooseACustomer sealed
	{
		//static DependencyProperty^ salesviewmodelProperty;
		
		static DependencyProperty^ customerviewmodelProperty;
		static DependencyProperty^ customerProperty;
		static DependencyProperty^ costcenterProperty;
		static DependencyProperty^ contactProperty;

		long lVisibleOrgsCount = 0;


	public:
		ChoooseACustomer();

		static void Register();

		//static property DependencyProperty^ SalesViewModelProperty
		//{
		//	DependencyProperty^ get() { return salesviewmodelProperty; }
		//}

		static property DependencyProperty^ CustomerViewmodelProperty
		{
			DependencyProperty^ get() { return customerviewmodelProperty; }
		}

		static property DependencyProperty^ CustomerProperty
		{
			DependencyProperty^ get() { return customerProperty; }
		}

		static property DependencyProperty^ CostCenterProperty
		{
			DependencyProperty^ get() { return costcenterProperty; }
		}

		static property DependencyProperty^ ContactProperty
		{
			DependencyProperty^ get() { return contactProperty; }
		}


		//property SaleOrderViewModel^ SalesViewmodel
		//{
		//	SaleOrderViewModel^ get()
		//	{
		//		return safe_cast<SaleOrderViewModel^>(GetValue(salesviewmodelProperty));
		//	}
		//	void set(SaleOrderViewModel^ val)
		//	{
		//		SetValue(salesviewmodelProperty, val);
		//	}
		//}

		property CustomerViewModel^ CustomerViewmodel
		{
			CustomerViewModel^ get()
			{
				return safe_cast<CustomerViewModel^>(GetValue(customerviewmodelProperty));
			}
			void set(CustomerViewModel^ val)
			{
				SetValue(customerviewmodelProperty, val);
			}
		}

		property CustomerOrg^ Customer
		{
			CustomerOrg^ get()
			{
				return safe_cast<CustomerOrg^>(GetValue(customerProperty));
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
				return safe_cast<COrgCostCenter^>(GetValue(costcenterProperty));
			}
			void set(COrgCostCenter^ val)
			{
				SetValue(costcenterProperty, val);
			}
		}
		property CustomerContact^ Contact
		{
			CustomerContact^ get()
			{
				return safe_cast<CustomerContact^>(GetValue(contactProperty));
			}
			void set(CustomerContact^ val)
			{
				SetValue(contactProperty, val);
			}
		}

	private:
		bool HasPatternMatchedOrg(CustomerDtosGroupedBindable^ org, String^ pattern);
		void DisplayOnlyMatchingOrgs(String^ pattern);
		void txtFindCustomer_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void rdContact_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void rdCostCenter_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnSelectCustomer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
