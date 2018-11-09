#pragma once

#include "typedefinitions.h"

using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;

namespace scwin
{
	[Bindable ]
	public ref class PricePlanType sealed 
	{
	private:
		bool bIsSelected;
	public:

		property String^ Name;
		property bool IsSelected
		{
			bool get() { return bIsSelected; }
			void set(bool val)
			{
				bIsSelected = val;
			}
		}

		void RadioButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
		{
			PubSubService::Instance->Publish(this, PubSubMessageIds::PricePlanSelected, this);
		}
	};

	[Bindable]
	public ref class PricePlanViewModelFacade sealed : public DependencyObject, public INotifyPropertyChanged
	{
		Product^ product;
		ProductPricePlan^ ppplan;
		IVectorView<PricePlanType^>^ planTypes;
		GenericPriceDetailsList^ priceDetails;
		//GenericPriceDetailsList^ mementoPriceDetails;
		bool isTieredPlan = false;


	private:
		void RaisePropertyChanged(String^ propName);
		void LoadPricePlanAsync();
		void LoadPricePlan();

	public:
		PricePlanViewModelFacade(Product^ product);
		virtual ~PricePlanViewModelFacade();

		property ProductPricePlan^ PricePlan
		{
			ProductPricePlan^ get() { return ppplan; }
			void set(ProductPricePlan^ val)
			{
				if (ppplan != val)
				{
					ppplan = val;
					RaisePropertyChanged("PricePlan");
				}
			}
		}

		property IVectorView<PricePlanType^>^ PlanTypes
		{
			IVectorView<PricePlanType^>^ get() { return planTypes; }
			void set(IVectorView<PricePlanType^>^ val)
			{
				if (planTypes != val)
				{
					planTypes = val;
					RaisePropertyChanged("PlanTypes");
				}
			}
		}

		property GenericPriceDetailsList^ PriceDetails
		{
			GenericPriceDetailsList ^ get() { return priceDetails; }
			void set(GenericPriceDetailsList^ val)
			{
				if (priceDetails != val)
				{
					priceDetails = val;
					RaisePropertyChanged("PriceDetails");
				}
			}
		}

		// returns standard price or defailt first price in the tiered list.
		property IPricePlanDetails^ DefaultPrice
		{
			IPricePlanDetails^ get()
			{
				if (priceDetails != nullptr && priceDetails->Size > 0)
				{
					return priceDetails->First()->Current;
				}
				return nullptr;
			}
		}

		property bool IsTieredPricePlan
		{
			bool get()
			{
				return isTieredPlan;
			}
			void set(bool val)
			{
				if (isTieredPlan != val)
				{
					isTieredPlan = val;
					RaisePropertyChanged("IsTieredPricePlan");
				}
			}
		}

		void PricePlanSelectionChanged(PricePlanType^ selected);

		void AddTierPriceWhenTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		void RemoveTierPriceWhenTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		String^ GetPricePlanType();

		bool ViewStateNoPricePlan();

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};


}
