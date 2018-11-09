//
// ProductsSelectList.xaml.h
// Declaration of the ProductsSelectList class
//

#pragma once
#include "DTOWrappers.h"
#include "ProductsSelectList.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductsSelectList sealed 
	{
		static DependencyProperty^ selectedProductsProperty;
		static DependencyProperty^ allProductsProperty;

	public:

		static void Register();

		ProductsSelectList();

		static property DependencyProperty^ SelectedProductsProperty {DependencyProperty^ get() { return selectedProductsProperty; }}
		static property DependencyProperty^ AllProductsProperty {DependencyProperty^ get() { return allProductsProperty; }}

		property IVector<Product^>^ SelectedProducts
		{
			IVector<Product^>^ get() 
			{
				return dynamic_cast<IVector<Product^>^>(GetValue(selectedProductsProperty)); 
			}
			void set(IVector<Product^>^ val)
			{
				SetValue(selectedProductsProperty, val);
			}
		}

		property IVector<ProductSelectable^>^ AllProducts
		{
			IVector<ProductSelectable^>^ get() 
			{ 
				return  dynamic_cast<IVector<ProductSelectable^>^>(GetValue(allProductsProperty)); 
			}
			void set(IVector<ProductSelectable^>^ val)
			{
				SetValue(allProductsProperty, val);
			}
		}

	private:

		void txtFindProduct_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void btnRemoveProduct_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CheckBox_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CheckBox_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
