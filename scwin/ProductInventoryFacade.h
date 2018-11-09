#pragma once

#include "typedefinitions.h"

namespace scwin
{

	public ref class ProductInventoryFacade sealed : public INotifyPropertyChanged
	{
		Product^ product;
		ProductInventory^ inventory;
		ProductInventoryProperties^ properties;

	public:
		ProductInventoryFacade(Product^ product);

		property Product^ SelectedProduct
		{
			Product^ get() { return product; }
		}

		property ProductInventory^ Inventory
		{
			ProductInventory^ get() { return inventory; }
		}

		property ProductInventoryProperties^ Properties
		{
			ProductInventoryProperties^ get() { return properties; }
		}

		property bool IsSelected;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;
	};

}
