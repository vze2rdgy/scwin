#pragma once
#include "dto.h"

namespace scwin
{
	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductInventory sealed :
		public IDTOBase
	{
		IMap<String^, uint64>^ perLocation;
		String^ pid;

	public:
		ProductInventory();

		// Inherited via IDTOBase
		virtual property DataState ObjectState;
		virtual property bool IsEmpty
		{
			bool get()
			{
				return false;
			}
		}
		virtual property bool IsValid
		{
			bool get()
			{
				return true;
			}
		}

		property IMap<String^, uint64>^ QuantityPerLocation
		{
			IMap<String^, uint64>^ get()
			{ 
				return perLocation; 
			}
			void set(IMap<String^, uint64>^ val)
			{
				if (perLocation != val || (perLocation != nullptr && val != nullptr && perLocation->Size != val->Size))
				{
					RaisePropertyChanged(PROPERTYNAME(QuantityPerLocation));
				}
			}
		}

		property String^ ProductId
		{
			String^ get()
			{
				return pid;
			}
			void set(String^ val)
			{
				if (pid != val)
				{
					pid = val;
					RaisePropertyChanged(PROPERTYNAME(ProductId));
				}
			}
		}

		property uint64 AvailableQuantity
		{
			uint64 get()
			{
				auto qty = 0UL;
				for (auto k : perLocation)
				{
					qty += k->Value;
				}
				return qty;
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;


	private:
		DECLARERAISEPROPCHANGED;
	};
}

