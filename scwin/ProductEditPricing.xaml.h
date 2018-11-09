//
// ProductEditPricing.xaml.h
// Declaration of the ProductEditPricing class
//

#pragma once
#include "PricePlanDetailsTemplateSelector.h"
#include "PricePlanLookup.xaml.h"
#include "ProductEditPricing.g.h"
#include "SharedViewModels.h"
#include "ProductViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductEditPricing sealed
	{
		String^ lastViewState = "NoDataState";
		EventRegistrationToken propChangedTok;

	public:
		ProductEditPricing();

		property ProductViewModel^ ViewModel;

	protected:
		virtual void OnApplyTemplate() override;

	private:
		void btnPricePlanEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnPricePlanSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnPricePlanCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void btnRemoveTierPrice_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnPricePlanLookup_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
