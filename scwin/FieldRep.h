#pragma once

#include "UserProfile.h"
#include "COrgCostCenter.h"

namespace scwin
{
	[Bindable]
	public ref class FieldRep sealed : IDTOBase
	{
		String^ id;
		String^ uid;
		String^ corgccid;
		String^ scode;
		String^ srole;
		String^ notes;

	private:
		void RaisePropertyChanged(String ^ propName);

	public:
		FieldRep();
		FieldRep(String^ id);
		virtual ~FieldRep();

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ UserProfileId
		{
			String^ get() 
			{
				return uid;
			}
			void set(String^ val)
			{
				if (uid != val)
				{
					uid = val;
					RaisePropertyChanged("UserProfileId");
				}
			}
		}

		property String^ CostCenterId
		{
			String^ get()
			{
				return corgccid;
			}
			void set(String^ val)
			{
				if (corgccid != val)
				{
					uid = val;
					RaisePropertyChanged("CostCenterId");
				}
			}
		}

		property String^ RepCode
		{
			String^ get()
			{
				return scode;
			}
			void set(String^ val)
			{
				if (scode != val)
				{
					scode = val;
					RaisePropertyChanged("RepCode");
				}
			}
		}

		property String^ RepRole
		{
			String^ get()
			{
				return srole;
			}
			void set(String^ val)
			{
				if (srole != val)
				{
					srole = val;
					RaisePropertyChanged("RepRole");
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
				if (notes != val)
				{
					notes = val;
					RaisePropertyChanged("Notes");
				}
			}
		}

		property IAsyncOperation<UserProfile^>^ User
		{
			IAsyncOperation<UserProfile^>^ get();
		}

		property IAsyncOperation<COrgCostCenter^>^ CostCenter
		{
			IAsyncOperation<COrgCostCenter^>^ get();
		}

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty 
		{
			bool get()
			{
				return true;
			}
		}
		
		virtual property bool IsValid
		{
			bool get()
			{
				if (StringHelpers::IsNullOrEmpty(uid) || StringHelpers::IsNullOrEmpty(corgccid))
					return false;
				return true;
			}
		}
		
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};

}
