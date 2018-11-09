#pragma once

#include "dto.h"

namespace scwin
{
	[Bindable]
	public ref class COrgCostCenter sealed : public IDTOBase
	{
		String^ id;
		String^ corgid;
		String^ name;
		String^ code;
		String^ paymentMethodCode;
		String^ paymenttermCode;
		String^ invoicedesccode;
		int creditTerm; // in days
		double creditLimit;
		String^ shippingAgentCode;
		String^ phone;
		String^ email;
		String^ fax;
		String^ mailaddr;
		String^ billaddr;
		String^ shipaddr;
		String^ searchName;
		String^ notes;
		bool isactive;
		bool isDefault;

	public:

		COrgCostCenter();

		COrgCostCenter(String^ id);

		virtual ~COrgCostCenter();

		property String^ Id
		{
			String^ get()
			{
				return id;
			}
		}

		property String^ ParentOrgId
		{
			String^ get()
			{
				return corgid;
			}
			void set(String^ val)
			{
				if (val != corgid)
				{
					corgid = val;
					RaisePropertyChanged("ParentOrgId");
				}
			}
		}

		property String^ Name
		{
			String^ get()
			{
				return name;
			}
			void set(String^ val)
			{
				if (val != name)
				{
					name = val;
					RaisePropertyChanged("Name");
				}
			}
		}

		property String^ Code
		{
			String^ get()
			{
				return code;
			}

			void set(String^ val)
			{
				if (val != code)
				{
					code = val;
					RaisePropertyChanged("Code");
				}
			}
		}

		property String^ PaymentMethodCode
		{
			String^ get()
			{
				return paymentMethodCode;
			}
			void set(String^ val)
			{
				if (val != paymentMethodCode)
				{
					paymentMethodCode = val;
					RaisePropertyChanged("PaymentMethodCode");
				}
			}
		}

		property String^ PaymentTermCode
		{
			String^ get()
			{
				return paymenttermCode;
			}
			void set(String^ val)
			{
				if (val != paymenttermCode)
				{
					paymenttermCode = val;
					RaisePropertyChanged("PaymentTermCode");
				}
			}
		}

		property String^ InvoiceDescCode
		{
			String^ get()
			{
				return invoicedesccode;
			}
			void set(String^ val)
			{
				if (val != invoicedesccode)
				{
					invoicedesccode = val;
					RaisePropertyChanged("InvoiceDescCode");
				}
			}
		}


		property int CreditTerm
		{
			int get()
			{
				return creditTerm;
			}
			void set(int val)
			{
				if (val != creditTerm)
				{
					creditTerm = val;
					RaisePropertyChanged("CreditTerm");
				}
			}
		}


		property double CreditLimit
		{
			double get()
			{
				return creditLimit;
			}
			void set(double val)
			{
				if (val != creditLimit)
				{
					creditLimit = val;
					RaisePropertyChanged("CreditLimit");
				}
			}
		}


		property String^ ShippingAgentCode
		{
			String^ get()
			{
				return shippingAgentCode;
			}
			void set(String^ val)
			{
				if (val != shippingAgentCode)
				{
					shippingAgentCode = val;
					RaisePropertyChanged("ShippingAgentCode");
				}
			}
		}


		property String^ Phone
		{
			String^ get()
			{
				return phone;
			}
			void set(String^ val)
			{
				if (val != phone)
				{
					phone = val;
					RaisePropertyChanged("Phone");
				}
			}
		}


		property String^ Email
		{
			String^ get()
			{
				return email;
			}
			void set(String^ val)
			{
				if (val != email)
				{
					email = val;
					RaisePropertyChanged("Email");
				}
			}
		}


		property String^ Fax
		{
			String^ get()
			{
				return fax;
			}
			void set(String^ val)
			{
				if (val != fax)
				{
					fax = val;
					RaisePropertyChanged("Fax");
				}
			}
		}


		property String^ MailingAddress
		{
			String^ get()
			{
				return mailaddr;
			}
			void set(String^ val)
			{
				if (val != mailaddr)
				{
					mailaddr = val;
					RaisePropertyChanged("MailingAddress");
				}
			}
		}

		property String^ BillingAddress
		{
			String^ get()
			{
				return billaddr;
			}
			void set(String^ val)
			{
				if (val != billaddr)
				{
					billaddr = val;
					RaisePropertyChanged("BillingAddress");
				}
			}
		}

		property String^ ShippingAddress
		{
			String^ get()
			{
				return shipaddr;
			}
			void set(String^ val)
			{
				if (val != shipaddr)
				{
					shipaddr = val;
					RaisePropertyChanged("ShippingAddress");
				}
			}
		}


		property String^ SearchName
		{
			String^ get()
			{
				return searchName;
			}
			void set(String^ val)
			{
				if (val != searchName)
				{
					searchName = val;
					RaisePropertyChanged("SearchName");
				}
			}
		}


		property String^ Notes
		{
			String^ get()
			{
				return notes;
			}
			void set(String^ val)
			{
				if (val != notes)
				{
					notes = val;
					RaisePropertyChanged("Notes");
				}
			}
		}

		property bool IsActive
		{
			bool get() { return isactive; }
			void set(bool val)
			{
				if (val != isactive)
				{
					isactive = val;
					RaisePropertyChanged("IsActive");
				}
			}
		}

		property bool IsDefault
		{
			bool get() { return isDefault; }
		internal:
			void set(bool val) { isDefault = val; }
		}


		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty { bool get() { return false; } }
		virtual property bool IsValid 
		{
			bool get() 
			{
				if (
					StringHelpers::IsNullOrEmpty(corgid) || 
					StringHelpers::IsNullOrEmpty(name) || 
					StringHelpers::IsNullOrEmpty(phone)
					)
					return false;
				return true;
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		void RaisePropertyChanged(String ^ propName);


	};

}