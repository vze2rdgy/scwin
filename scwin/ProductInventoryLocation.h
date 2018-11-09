#pragma once
#include "dto.h"
#include "Address.h"

namespace scwin
{
	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductInventoryLocation sealed :
		public IDTOBase
	{
		String^ id;
		String^ code;
		String^ name;
		String^ contactId;
		scwin::Address^ address;

		DataState objState;

	public:
		ProductInventoryLocation();
		ProductInventoryLocation(String^ id);

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ Code
		{
			String^ get() { return code; }
			void set(String^ val)
			{
				if (code != val)
				{
					code = val;
					RaisePropertyChanged(PROPERTYNAME(Code));
				}
			}
		}

		property String^ Name
		{
			String^ get() { return name; }
			void set(String^ val)
			{
				if (name != val)
				{
					name = val;
					RaisePropertyChanged(PROPERTYNAME(Name));
				}
			}
		}

		property String^ ContactId
		{
			String^ get() { return contactId; }
			void set(String^ val)
			{
				if (contactId != val)
				{
					contactId = val;
					RaisePropertyChanged(PROPERTYNAME(ContactId));
				}
			}
		}

		property scwin::Address^ Address
		{
			scwin::Address^ get();
			void set(scwin::Address^);
		}

		property bool IsSelected;

		// Inherited via IDTOBase
		virtual property DataState ObjectState
		{
			DataState get();
			void set(DataState);
		}

		virtual property bool IsEmpty
		{
			bool get()
			{
				return false;
			}
		}

		virtual property bool IsValid
		{
			bool get();
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;

		ProductInventoryLocation^ Clone();


	private:
		DECLARERAISEPROPCHANGED;

	};

}
