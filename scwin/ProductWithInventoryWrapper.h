#pragma once

namespace scwin
{

	public ref class ProductWithInventoryWrapper sealed : public INotifyPropertyChanged
	{
		Product^ product;
		PricePlanViewModelFacade^ priceplan;
		ProductInventoryFacade^ inventory;

	public:
		ProductWithInventoryWrapper(Product^ product);

		property Product^ ProductItem
		{
			Product^ get() { return product; }
			void set(Product^ val)
			{
				if (product != val)
				{
					product = val;
					RaisePropertyChanged(PROPERTYNAME(ProductItem));
				}
			}
		}

		// reusing PricePlanViewModelFacade
		property PricePlanViewModelFacade^ PricePlan
		{
			PricePlanViewModelFacade^ get();
			void set(PricePlanViewModelFacade^ val);
		}

		property ProductInventoryFacade^ Inventory
		{
			ProductInventoryFacade^ get() { return inventory; }
		}

	internal:

		void LoadPriceInfo();
		void LoadInventoryInfo();

	private:
		DECLARERAISEPROPCHANGED;


	public:
		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		static IAsyncOperation<IVector<ProductWithInventoryWrapper^>^>^ BuildList(ProductChooseOptions options);

	};

}