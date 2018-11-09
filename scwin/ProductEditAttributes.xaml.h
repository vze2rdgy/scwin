//
// ProductEditAttributes.xaml.h
// Declaration of the ProductEditAttributes class
//

#pragma once

#include "ProductEditAttributes.g.h"
#include "SharedViewModels.h"
#include "ProductViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductEditAttributes sealed
	{
	private:
		EventRegistrationToken asbtxtchangedtok;

	public:
		ProductEditAttributes();
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	
		property ProductViewModel^ ViewModel;

	protected:
		virtual void OnApplyTemplate() override;

	private:
		void btnPricePlanEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnPricePlanSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnPricePlanCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void txtFamily_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void txtFamily_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void txtFamily_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args);
		void btnRemoveAttribute_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnSelectAddAttribute_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnFamilyLookup_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnDefaultAttrib_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void txtFamily_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
