#pragma once
#include "dto.h"

namespace scwin
{

	/// <summary>Represents single price configuration. </summary>
	[Bindable]
	public ref class PricePlanStandardDetails sealed :
		public IPricePlanDetails
	{
		String^ id;
		String^ priceplanId;
		float price;

	private:
		void RaisePropertyChanged(String^ propName);

	public:
		PricePlanStandardDetails();
		PricePlanStandardDetails(String^ id);

		virtual property bool CanRaiseEvents;

		// Inherited via IDTOBase
		virtual property DataState ObjectState;
		virtual property bool IsEmpty { bool get(){ return false; } }

		virtual property bool IsValid
		{
			bool get();
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



	};

}