//
// ProductFamilies.xaml.h
// Declaration of the ProductFamilies class
//

#pragma once

#include "ProductFamilies.g.h"
#include "DTOWrappers.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductFamilies sealed : INotifyPropertyChanged
	{
		IVector<ProductFamily^>^ allFamilies;
		ProductFamily^ selectedFamily;
		UINT lastSelectedIndex;
		ProductAttribute^ defAttrib;
		GeneralAttributeValue^ defAttribVal;
		bool bApplyToAllMemberProducts;
		static DependencyProperty^ extendedProperty;
		typedef vector<Product^> VProduct;
		VProduct removeUndoList;
		EventRegistrationToken selectedProductsChanged;


	private:
		void RaisePropertyChanged(String^ propName);

		void txtFindFamily_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void listOfFamilies_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void UpdateSelectedItem(int selectedIndex);


		void btnAddFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnSaveFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void ListProductsInFamily(String^ familyId);


		void btnUndoFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnDeleteFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	public:

		static void Register();

		ProductFamilies();

		property DependencyProperty^ ExtendedProperty
		{
			DependencyProperty^ get() { return extendedProperty; }
		}

		property bool Extended
		{
			bool get() { return (bool)GetValue(extendedProperty); }
			void set(bool val) { SetValue(extendedProperty, val); }
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property IVector<ProductFamily^>^ AllFamilies
		{
			IVector<ProductFamily^>^ get()
			{
				return allFamilies;
			}
		private:
			void set(IVector<ProductFamily^>^ val)
			{
				if (allFamilies != val)
				{
					allFamilies = val;
					RaisePropertyChanged("AllFamilies");
				}
			}
		}

		property IVector<ProductSelectable^>^ AllProducts;

		property IVector<Product^>^ SelectedProducts;

		property ProductFamily^ SelectedFamily
		{
			ProductFamily^ get()
			{
				return selectedFamily;
			}
			void set(ProductFamily^ val)
			{
				if (selectedFamily != val)
				{
					selectedFamily = val;
					RaisePropertyChanged("SelectedFamily");
					if (val != nullptr)
						ListProductsInFamily(val->Id);
				}
			}
		}

		property ProductAttribute^ DefaultAttribute
		{
			ProductAttribute^ get() { return defAttrib; }
			void set(ProductAttribute^ val) {
				if (defAttrib != val)
				{
					defAttrib = val;
					RaisePropertyChanged("DefaultAttribute");
				}
			}
		}

		property GeneralAttributeValue^ DefaultAttributeValue
		{
			GeneralAttributeValue^ get() { return defAttribVal; }
			void set(GeneralAttributeValue^ val) {
				if (defAttribVal != val)
				{
					defAttribVal = val;
					RaisePropertyChanged("DefaultAttributeValue");
				}
			}
		}

		IVector<String^>^ GetAttributeTypes()
		{
			return scMainViewModel::GetProductAttributeTypes();
		}

		property bool ApplyToAllMemberProducts
		{
			bool get() {
				return bApplyToAllMemberProducts;
			}
			void set(bool val)
			{
				if (bApplyToAllMemberProducts != val)
				{
					bApplyToAllMemberProducts = val;
					RaisePropertyChanged("ApplyToAllMemberProducts");
				}
			}
		}

		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	private:
		void txtAttribuetName_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void txtAttribuetName_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args);
		void txtAttribuetName_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void txtAttrValueName_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void txtAttrValueName_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args);
		void txtAttrValueName_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void txtAttrValueUom_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void txtAttrValueUom_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args);
		void txtAttrValueUom_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);

		void ClearValues();

		void btnAddProductsToFamily_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ProductSelectList_Closed(Platform::Object^ sender, Platform::Object^ e);
		void btnRemoveProduct_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void SelectedProductsListChanged(IObservableVector<Product^>^ sender, IVectorChangedEventArgs^ args);
	};
}
