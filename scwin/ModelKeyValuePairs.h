#pragma once

#include "CustomerContact.h"

namespace scwin
{
	///////////// AddressKeyValuePair ////////////////////
	[Bindable]
	public ref class AddressKeyValuePair sealed : public IKeyValuePair<String^, Address^>, public INotifyPropertyChanged
	{
		String^ _ccid;
		String^ key;
		Address^ value;
		bool showAddrOptions = false;
		String^ addrOptionSelected;
		bool canDisplayDeleteButton = false;
		IVector<String^>^ _addressTypes;

	public:

		AddressKeyValuePair(String^ key, Address^ value, String^ ccid);

		// Inherited via IKeyValuePair
		virtual property Platform::String ^ Key
		{
			String^ get() { return key; }
		}

		property String^ CostcenterId
		{
			String^ get() { return _ccid; }
		}

		virtual property scwin::Address ^ Value
		{
			Address^ get()
			{
				return value;
			}
		}

		property IVector<String^>^ AddressSameAsOptions
		{
			IVector<String^>^ get() {
				return _addressTypes;
			}
		}

		property bool ShowAddressOptions
		{
			bool get() { return showAddrOptions; }
			void set(bool val)
			{
				showAddrOptions = val;
				RaisePropertyChanged("ShowAddressOptions");
			}
		}

		property String^ AddressOptionSelected
		{
			String^ get()
			{
				return addrOptionSelected;
			}
			void set(String^ val)
			{
				if (val != addrOptionSelected)
				{
					addrOptionSelected = val;
					RaisePropertyChanged("AddressOptionSelected");
				}
			}
		}

		property bool CanDisplayDeleteButton
		{
			bool get() {
				return canDisplayDeleteButton;
			}
			void set(bool val)
			{
				if (canDisplayDeleteButton != val)
				{
					canDisplayDeleteButton = val;
					RaisePropertyChanged("CanDisplayDeleteButton");
				}
			}
		}


		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;


	private:
		void RaisePropertyChanged(String^ propName);

	};


	////////////////////CustomerContactKeyValuePair////////////////////////////

	ref class CostCenterDtosGroupedBindable;

	public ref class CustomerContactKeyValuePair sealed : 
		public IKeyValuePair<UINT, CustomerContact^>, public INotifyPropertyChanged
	{
		UINT pos;
		CostCenterDtosGroupedBindable^ _cc;
		CustomerContact^ value;
		bool isContactSelectedForSalesOrder = false;

	public:

		CustomerContactKeyValuePair(UINT pos, CustomerContact^ value, CostCenterDtosGroupedBindable^ cc);

		// Inherited via IKeyValuePair
		virtual property UINT Key
		{
			UINT get() { return pos; }
		}

		property CostCenterDtosGroupedBindable^ Costcenter
		{
			CostCenterDtosGroupedBindable^ get() { return _cc; }
		}

		virtual property scwin::CustomerContact ^ Value
		{
			CustomerContact^ get()
			{
				return value;
			}
		}

		property bool IsContactSelectedForSalesOrder
		{
			bool get() {
				return isContactSelectedForSalesOrder
					;
			}
			void set(bool val)
			{
				if (isContactSelectedForSalesOrder != val)
				{
					isContactSelectedForSalesOrder = val;
					RaisePropertyChanged("IsContactSelectedForSalesOrder");
				}
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		bool Save();
		bool Cancel();

	internal:
		void SetKey(UINT newkey)
		{
			pos = newkey;
			RaisePropertyChanged("Key");
		}

	private:
		void RaisePropertyChanged(String^ propName);

	};
}