//
// ChooseAProduct.xaml.h
// Declaration of the ChooseAProduct class
//

#pragma once

#include "ProductWithInventoryWrapper.h"
#include "ChooseAProduct.g.h"
#include "SaleOrderViewModel.h"

namespace scwin
{
	public delegate void ProductSelectedEventHandler(ChooseAProduct^ sender, ProductWithInventoryWrapper^ productWrapper);

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ChooseAProduct sealed : public INotifyPropertyChanged
	{
		static DependencyProperty^ productlistProperty;
		static DependencyProperty^ selectedProductProperty;
		static DependencyProperty^ filterProperty;
		static DependencyProperty^ displaydetailOptionsProperty;
		event ProductSelectedEventHandler^ productSelectedEvent;
		IVector<ProductWithInventoryWrapper^>^ productPresentationList;

	public:
		ChooseAProduct();

		static void Register();

		static property DependencyProperty^ SelectedProductProperty
		{
			DependencyProperty^ get() { return selectedProductProperty; }
		}

		property ProductWithInventoryWrapper^ SelectedProduct
		{
			ProductWithInventoryWrapper^ get()
			{
				return (ProductWithInventoryWrapper^)GetValue(selectedProductProperty);
			}
			void set(ProductWithInventoryWrapper^ val)
			{
				SetValue(selectedProductProperty, val);
			}
		}

		static property DependencyProperty^ ProductListProperty
		{
			DependencyProperty^ get() { return productlistProperty; }
		}

		property IVector<Product^>^ ProductList
		{
			IVector<Product^>^ get()
			{ 
				return (IVector<Product^>^)GetValue(productlistProperty);
			}
			void set(IVector<Product^>^ val)
			{
				SetValue(productlistProperty, val);
			}
		}

		property IVector<ProductWithInventoryWrapper^>^ ProductPresentationList
		{
			IVector<ProductWithInventoryWrapper^>^ get() { return productPresentationList; }
		private:
			void set(IVector<ProductWithInventoryWrapper^>^ val)
			{
				if (productPresentationList != val)
				{
					productPresentationList = val;
					RaisePropertyChanged(PROPERTYNAME(ProductPresentationList));
				}
			}
		}

		static property DependencyProperty^ FilterProperty
		{
			DependencyProperty^ get() { return filterProperty; }
		}

		property String^ Filter
		{
			String^ get() { return (String^)GetValue(filterProperty); }
			void set(String^ val)
			{
				SetValue(filterProperty, val);
			}
		}

		static property DependencyProperty^ DisplayDetailOptionsProperty
		{
			DependencyProperty^ get() 
			{
				return displaydetailOptionsProperty;
			}
		}

		property ProductChooseOptions DisplayDetailOptions
		{
			ProductChooseOptions get()
			{
				return (ProductChooseOptions)GetValue(displaydetailOptionsProperty);
			}
			void set(ProductChooseOptions val)
			{
				SetValue(displaydetailOptionsProperty, val);
			}
		}

		event ProductSelectedEventHandler^ ProductSelected
		{
			Windows::Foundation::EventRegistrationToken add(ProductSelectedEventHandler^ handler)
			{
				return productSelectedEvent += handler;
			}

			void remove(Windows::Foundation::EventRegistrationToken token)
			{
				productSelectedEvent -= token;
			}

			void raise(ChooseAProduct^ sender, ProductWithInventoryWrapper^ prod)
			{
				return productSelectedEvent(sender, prod);
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;

		static void OnFilterPropertyChanged(DependencyObject^ object, DependencyPropertyChangedEventArgs^ args);

		void OnLoad(Object^ sender, RoutedEventArgs^ eargs);

		void LoadProductsBasicInfo();
		void LoadProductsWithPriceInfo();
		void LoadProductsWithPriceAndInventoryInfo();

		void lvProducts_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
};
}
