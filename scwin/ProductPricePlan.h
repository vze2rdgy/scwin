#pragma once
#include "dto.h"

namespace scwin
{
	public interface struct IPricePlanDetails : IDTOBase
	{
		property String^ Id
		{
			String^ get();
			void set(String^ id);
		}

		property String^ PricePlanId
		{
			String^ get();
			void set(String^ val);
		}

		property float Price
		{
			float get();
			void set(float val);
		}
	};

	[Bindable]
	public ref class ProductPricePlan sealed : IDTOBase
	{
		String^ id;
		String^ code;
		String^ prodid;
		String^ desc;
		DateTime dateInit;
		DateTime dateEnded;
		String^ currency;

	private:
		void RaisePropertyChanged(String^ propName);

	public:
		ProductPricePlan();

		ProductPricePlan(String^ id);

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		virtual property bool CanRaiseEvents;

		property String^ Id
		{
			String^ get()
			{
				return id;
			}
			void set(String^ val)
			{
				if (id != val)
				{
					id = val;
					this->RaisePropertyChanged("Id");
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
				if (code != val)
				{
					code = val;
					this->RaisePropertyChanged("Code");
				}
			}
		}

		property String^ Description
		{
			String^ get() { return desc; }
			void set(String^ val)
			{
				if (desc != val)
				{
					desc = val;
					RaisePropertyChanged("Description");
				}
			}
		}

		property DateTime StartDate
		{
			DateTime get() { return dateInit; }
			void set(DateTime val)
			{
				if (dateInit.UniversalTime != val.UniversalTime)
				{
					dateInit = val;
					RaisePropertyChanged("StartDate");
				}
			}
		}

		property DateTime EndDate
		{
			DateTime get() { return dateEnded; }
			void set(DateTime val)
			{
				if (dateEnded.UniversalTime != val.UniversalTime)
				{
					dateEnded = val;
					RaisePropertyChanged("EndDate");
				}
			}
		}

		property String^ Currency
		{
			String^ get()
			{
				return currency;
			}
			void set(String^ val)
			{
				if (currency != val)
				{
					currency = val;
					RaisePropertyChanged("Currency");
				}
			}
		}

		// Inherited via IDTOBase
		virtual property DataState ObjectState;

		virtual property bool IsEmpty
		{
			bool get();
		}

		virtual property bool IsValid
		{
			bool get();
		}

	};
}