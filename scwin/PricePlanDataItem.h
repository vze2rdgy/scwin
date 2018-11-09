#pragma once

namespace scwin
{
	[Bindable]
	public ref class PricePlanDataItem : DependencyObject, INotifyPropertyChanged
	{
		ProductPricePlan^ plan;
		String^ planType;
		GenericPriceDetailsList^ priceDetails;

		void RaisePropertyChanged(String^ propName);

	public:

		property ProductPricePlan^ Plan
		{
			ProductPricePlan^ get()
			{
				return plan;
			}
			void set(ProductPricePlan^ val)
			{
				plan = val;
				RaisePropertyChanged("Plan");
			}
		}

		property String^ PlanType
		{
			String^ get() { return planType; }
			void set(String^ val) { planType = val; }
		}

		property GenericPriceDetailsList^ DetailsList
		{
			GenericPriceDetailsList^ get()
			{
				return priceDetails;
			}
			void set(GenericPriceDetailsList^ val)
			{
				priceDetails = val;
				RaisePropertyChanged("DetailsList");
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	};
}