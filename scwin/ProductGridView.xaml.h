//
// Products.xaml.h
// Declaration of the Products class
//

#pragma once

#include "LayoutAwarePage.h"
#include "ProductCategory.h"
#include "Product.h"
#include "ProductGridView.g.h"
#include "Enums.h"
#include "ProductImageAvailabilityConverte.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;


namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductGridView sealed : INotifyPropertyChanged
	{
		Array<byte>^ imgBinary;
		int static const chunkSize = 4096;
		int currChunk;
		String^ hexdump;
		event scwin::Action<String^>^ HexDumpCompleted;
		IObservableVector<ProductCategory^>^ catBreadcrump;
		Command^ cmdSelectMenuItem;
		String^ lastSortedBy;
		ListViewTypes lastViewType;
		Product^ selectedProduct;
		IObservableVector<Product^>^ allproducts;
		IObservableVector<Product^>^ displayedProducts; // product shown after a sorting or filtering. by default same as allproducts;

	protected:
		virtual void OnNavigatedTo(NavigationEventArgs^ e) override;

	public:
		ProductGridView();

		property ProductViewModel^ ViewModel;

		property Command^ SortMenuItemCommand;

		property Command^ SortCommand; // TODO: not required reuse it for another purpose if not remove it.

		property Command^ ViewTypeCommand;

		property IObservableVector<Product^>^ DisplayedProducts
		{
			IObservableVector<Product^>^ get() 
			{
				return this->displayedProducts; 
			}
			void set(IObservableVector<Product^>^ list)
			{
				if (list != displayedProducts)
				{
					displayedProducts = list;
					NotifyPropertyChange("DisplayedProducts");
				}
			}
		}

		property Product^ SelectedProduct
		{
			Product^ get() { return this->selectedProduct; }
			void set(Product^ val)
			{
				if (selectedProduct != val)
				{
					selectedProduct = val;
					NotifyPropertyChange("SelectedProduct");
				}
			}
		}

		property Array<byte>^ ImageBinary
		{
			Array<byte>^ get()
			{
				return imgBinary;
			}
		}

		property String^ Title
		{
			String^ get()
			{
				return "Products";
			}
		}

		property String^ CurrentSort
		{
			String^ get()
			{
				return lastSortedBy;
			}
			void set(String^ val)
			{
				lastSortedBy = val;
				NotifyPropertyChange("CurrentSort");
			}
		}

		property IObservableVector<ProductCategory^>^ CategoryBreadcrump
		{
		public:
			IObservableVector<ProductCategory^>^ get() { return catBreadcrump; }
		private:
			void set(IObservableVector<ProductCategory^>^ val)
			{
				if (val != catBreadcrump)
				{
					catBreadcrump = val;
					NotifyPropertyChange(L"CategoryBreadcrump");
				}
			}
		}

		virtual event PropertyChangedEventHandler^ PropertyChanged;

	private:

		void OnLoading();

		void PopulateProducts(String^ catId = nullptr);

		void ExecuteSortCommand(Object^ data);
		bool CanExecuteSortCommand(Object^ data);

		void ExecuteViewTypeCommand(Object^ data);
		bool CanExecuteViewTypeCommand(Object^ data);
		
		void ExecuteSortMenuItemCommand(Object^ data);
		bool CanExecuteSortMenuItemCommand(Object^ data);

		void ExecuteSelectMenuCommand(Object^ data);
		bool CanExecuteSelectMenuCommand(Object^ data);

		void NotifyPropertyChange(String^ propertyName);

		String^ ToHex(byte value);
		Platform::String^ GetAddress(int databyte);
		//void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void ReadBytes(DataReader^ reader);
		void OnHexDumpCompleted(Platform::String ^obj);
		void lstCategoryBreadCrump_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void HyperlinkButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void MenuFlyoutItem_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void listViewProducts_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void btnProductView_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnViewProduct_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		//void ProductItem_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		//void ProductItem_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
};
}
