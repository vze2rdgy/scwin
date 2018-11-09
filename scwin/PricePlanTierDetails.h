#pragma once
#include "ProductPricePlan.h"


namespace scwin
{
	/// <summary>Represents one of a multi-tier pricing configuration. </summary>
	[Bindable]
	public ref class PricePlanTierDetails sealed :
		public IPricePlanDetails
	{
		String^ id;
		String^ priceplanId;
		float price;
		uint64 beginQty;
		uint64 endQty;  // this field may not be necessary if strucure the data correctly.

	private:
		void RaisePropertyChanged(String^ propName);

	public:
		PricePlanTierDetails();
		PricePlanTierDetails(String^ id);

		virtual property bool CanRaiseEvents;

		// Inherited via IDTOBase
		virtual property DataState ObjectState;
		virtual property bool IsEmpty { bool get() { return false; } }

		virtual property bool IsValid
		{
			bool get()
			{
				return true;
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property String^ Id
		{
			virtual String^ get() { return id; }
			virtual void set(String^ val) {
				if (id != val)
				{
					id = val;
					RaisePropertyChanged("Id");
				}
			}
		}

		property String^ PricePlanId
		{
			virtual String^ get() { return priceplanId; }
			virtual void set(String^ val) {
				if (priceplanId != val)
				{
					priceplanId = val;
					RaisePropertyChanged("PricePlanId");
				}
			}
		}



		property float Price
		{
			virtual float get() { return price; }
			virtual void set(float val) 
			{
				if (price != val)
				{
					price = val;
					RaisePropertyChanged("Price");
				}
			}
		}

		property uint64 BeginQuantity
		{
			uint64  get() { return beginQty; }
			void set(uint64 val)
			{
				if (beginQty != val)
				{
					beginQty = val;
					RaisePropertyChanged("BeginQuantity");
				}
			}
		}

		property uint64 EndQuantity
		{
			uint64 get() { return endQty; }
			void set(uint64 val)
			{
				if (endQty != val)
				{
					endQty = val;
					RaisePropertyChanged("EndQuantity");
				}
			}
		}

	};


}
