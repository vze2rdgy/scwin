#pragma once

#include "dto.h"
#include "CustomerOrg.h"
#include "COrgCostCenter.h"
#include "CustomerContact.h"
#include "ModelKeyValuePairs.h"

namespace scwin
{

#define MAILING "Mailing"
#define SHIPPING "Shipping"
#define BILLING "Billing"

	ref class CustomerDtosGroupedBindable;

	/// <summary>
	/// Friendly class to bind related objects together for display purpose.
	/// </summary>
	[Bindable]
	public ref class CostCenterDtosGroupedBindable sealed : INotifyPropertyChanged
	{
		COrgCostCenter^ cc;
		bool islandscape = true;
		bool isValid = false;
		IVector<AddressKeyValuePair^>^ addresses;
		IVector<CustomerContactKeyValuePair^>^ contacts;
		bool isCostCenterSelectedForSalesOrder = false;

	private:
		void RaisePropertyChanged(String ^ propName);
		void BuildThisAsync();
		void Build();

	internal:
		CostCenterDtosGroupedBindable();
		CostCenterDtosGroupedBindable(COrgCostCenter^ cc);

	public:

		property CustomerDtosGroupedBindable^ Org;

		property bool IsLandscape
		{
			bool get() { return islandscape; }
			void set(bool val) 
			{
				islandscape = val;
				RaisePropertyChanged(PROPERTYNAME(IsLandscape));
			}
		}

		Windows::UI::Xaml::Visibility GetPositionVisibility()
		{
			return Position == 0 ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
		}

		property UINT Position;

		property COrgCostCenter^ CostCenter
		{
			COrgCostCenter^ get()
			{
				return cc;
			}

			void set(COrgCostCenter^ val)
			{
				if (cc != val)
				{
					cc = val;
					RaisePropertyChanged(PROPERTYNAME(CostCenter));
				}
			}
		}

		property bool IsValid
		{
			bool get();
			void set(bool val);
		}

		property IVector<AddressKeyValuePair^>^ Addresses
		{
			IVector<AddressKeyValuePair^>^ get() 
			{
				if (addresses == nullptr)
					addresses = ref new Vector<AddressKeyValuePair^>();
				return addresses; 
			}
			void set(IVector<AddressKeyValuePair^>^ val)
			{
				if (addresses != val)
				{
					addresses = val;
					RaisePropertyChanged(PROPERTYNAME(Addresses));
				}
			}
		}

		property AddressKeyValuePair^ MailingAddress
		{
			AddressKeyValuePair^ get();
		private:
			void set(AddressKeyValuePair^ val);
		}

		property AddressKeyValuePair^ BillingAddress
		{
			AddressKeyValuePair^ get();
			void set(AddressKeyValuePair^ val);
		}

		property AddressKeyValuePair^ ShippingAddress
		{
			AddressKeyValuePair^ get();
			void set(AddressKeyValuePair^ val);
		}

		property IVector<CustomerContactKeyValuePair^>^ Contacts
		{
			IVector<CustomerContactKeyValuePair^>^ get()
			{
				if (contacts == nullptr)
					contacts = ref new Vector<CustomerContactKeyValuePair^>();
				return contacts;
			}
			void set(IVector<CustomerContactKeyValuePair^>^ val)
			{
				if (contacts != val)
				{
					contacts = val;
					RaisePropertyChanged(PROPERTYNAME(Contacts));
				}
			}
		}

		property bool IsCostCenterSelectedForSalesOrder
		{
			bool get() {
				return isCostCenterSelectedForSalesOrder;
			}
			void set(bool val)
			{
				if (isCostCenterSelectedForSalesOrder != val)
				{
					isCostCenterSelectedForSalesOrder = val;
					RaisePropertyChanged(PROPERTYNAME(IsCostCenterSelectedForSalesOrder));
				}
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		bool Save();
		bool Cancel();

	//	property ICommand^ EditLocation;

	//private:
	//	void ExecEditLocation(Object^ param);
	//	bool CanExecEditLocation(Object^ param);


	};

	/// <summary>
	/// Friendly class to bind related objects together for display purpose.
	/// </summary>
	[Bindable]
	public ref class CustomerDtosGroupedBindable sealed : INotifyPropertyChanged
	{
	private:
		bool isValid = false;
		CustomerOrg^ corg;
		CostCenterDtosGroupedBindable^ defCostCenter;
		IVector<CostCenterDtosGroupedBindable^>^ costcenters;
		String^ subHeader;

	internal:
		CustomerDtosGroupedBindable();
		CustomerDtosGroupedBindable(CustomerOrg^ orgs);

	private:
		DECLARERAISEPROPCHANGED;

		void OnOrgPropertyChanged(Object^ sender, PropertyChangedEventArgs^ args);

	internal:
		task<MessageIds> ExecuteSaveCommand(Object^ param);
		bool CanExecuteSaveCommand(Object^ param);

	public:

		property UINT Position;

		property CustomerOrg^ Organization
		{
			CustomerOrg^ get()
			{
				return corg;
			}

			void set(CustomerOrg^ org)
			{
				if (corg != org)
				{
					corg = org;
					RaisePropertyChanged(PROPERTYNAME(Organization));
				}
			}
		}

		property String^ SubHeader
		{
			String^ get()
			{
				return subHeader;
			}
			void set(String^ val)
			{
				if (subHeader != val)
				{
					subHeader = val;
					RaisePropertyChanged(PROPERTYNAME(SubHeader));
				}
			}
		}

		property CostCenterDtosGroupedBindable^ DefaultCostCenter
		{
			CostCenterDtosGroupedBindable^ get() 
			{ 
				return defCostCenter; 
			}

			void set(CostCenterDtosGroupedBindable^ val)
			{
				if (val != defCostCenter)
				{
					defCostCenter = val;
					RaisePropertyChanged(PROPERTYNAME(DefaultCostCenter));
				}
			}
		}

		property IVector<CostCenterDtosGroupedBindable^>^ CostCenters
		{
			IVector<CostCenterDtosGroupedBindable^>^ get() 
			{
				if (costcenters == nullptr)
					costcenters = ref new Vector<CostCenterDtosGroupedBindable^>();

				return costcenters; 
			}
			void set(IVector<CostCenterDtosGroupedBindable^>^ val)
			{
				if (costcenters != val)
				{
					costcenters = val;
					RaisePropertyChanged(PROPERTYNAME(CostCenters));
				}
			}
		}


		property bool IsValid
		{
			bool get();
			void set(bool val);
		}

		/// <summary>Add a coster to current organization </summary>
		void AddCostCenter();

		/// <summary> 
		/// Create an empty organization instance, and related necessary default objects
		/// such as empty default cost center, empty default cost center contact, etc
		/// </summary>
		static IAsyncOperation<CustomerDtosGroupedBindable^>^ New();

		/// <summary>
		/// Builds complete group of dtos around an existing organization.
		/// </summary>
		void BuildThisAsync(CustomerOrg^ org);

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		bool SaveOrganization();
		IAsyncOperation<bool>^ SaveOrganizationAsync();

		///////////////////////// Below helper properties are used by Details View ////////////////////////////////

	private:
		CostCenterDtosGroupedBindable ^ currentCostCenter;

	public:
		property CostCenterDtosGroupedBindable^ CurrentCostCenter
		{
			CostCenterDtosGroupedBindable^ get()
			{
				return currentCostCenter;
			}

			void set(CostCenterDtosGroupedBindable^ val)
			{
				if (val != currentCostCenter)
				{
					currentCostCenter = val;
					RaisePropertyChanged(PROPERTYNAME(CurrentCostCenter));
				}
			}
		}

	};

}