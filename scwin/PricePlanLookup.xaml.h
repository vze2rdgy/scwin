//
// PricePlanLookup.xaml.h
// Declaration of the PricePlanLookup class
//

#pragma once

#include "PricePlanStandardDetails.h"
#include "PricePlanTierDetails.h"
#include "PricePlanDataItem.h"
#include "PricePlanDetailsCompactTemplateSelector.h"
#include "PricePlanLookup.g.h"

namespace scwin
{

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PricePlanLookup sealed : INotifyPropertyChanged
	{
		IVectorView<PricePlanDataItem^>^ pricePlanItems;
		void RaisePropertyChanged(String^ propName);

	public:
		PricePlanLookup();

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property IVectorView<PricePlanDataItem^>^ PricePlanitems
		{
			IVectorView<PricePlanDataItem^>^ get()
			{
				return pricePlanItems;
			}
			void set(IVectorView<PricePlanDataItem^>^ val)
			{
				pricePlanItems = val;
				RaisePropertyChanged("PricePlanItems");
			}
		}

	private:
		void AcceptButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void CloseButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};

}
