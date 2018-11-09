#pragma once

#include "SalesOrderDtos.h"
#include "SalesOrderWrapper.h"
#include "OrderGroupedInfoList.h"

#include "ProductViewModel.h"
#include "CustomerViewModel.h"

namespace scwin
{

	public ref class SaleOrderViewModel sealed : INotifyPropertyChanged
	{
	private:
		bool isLoaded = false;
		bool isOrderViewInListViewMode = false;
		SalesOrderWrapper^ current;

		IVector<SalesOrder^>^ baseOrders = nullptr;
		IVector<SalesOrderWrapper^>^ orders;
		IVector<OrderGroupedInfoList^>^ orderGroups;

		IVector<String^>^ sortSelections;
		String^ currentSortSelection;
		IVector<GeneralStatus^>^ orderStatusList;
		IVector<GeneralStatus^>^ paymentStatusList;
		CustomerViewModel^ custVm;
		ProductViewModel^ prodVm;

	public:
		SaleOrderViewModel();

		void LoadState();
		
		void SaveState();


		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property CustomerViewModel^ CustomerViewmodel 
		{
			CustomerViewModel^ get() 
			{ 
				return custVm; 
			}
			
			void set(CustomerViewModel^ val)
			{
				if (custVm != val)
				{
					custVm = val;
				}
			}
		}

		property ProductViewModel^ ProductViewmodel
		{
			ProductViewModel^ get() { return prodVm; }
			void set(ProductViewModel^ val)
			{
				if (prodVm != val)
				{
					prodVm = val;
				}
			}
		}

		property IVector<GeneralStatus^>^ OrderStatusList
		{
			IVector<GeneralStatus^>^ get();
		private:
			void set(IVector<GeneralStatus^>^);
		}

		property IVector<GeneralStatus^>^ PaymentStatusList
		{
			IVector<GeneralStatus^>^ get();
		private:
			void set(IVector<GeneralStatus^>^);
		}

		property bool IsLoaded
		{
			bool get() { return isLoaded; }
			void set(bool val)
			{
				if (isLoaded != val)
				{
					isLoaded = val;
					RaisePropertyChanged(PROPERTYNAME(IsLoaded));
				}
			}
		}

		property IVector<String^>^ SortSelections
		{
			IVector<String^>^ get() { return sortSelections; }
		}

		property String^ CurrentSortSelection
		{
			String^ get() 
			{
				return currentSortSelection;
			}
			void set(String^ val)
			{
				currentSortSelection = val;
				RaisePropertyChanged(PROPERTYNAME(CurrentSortSelection));
			}
		}


		property bool IsOrderviewInListViewMode
		{
			bool get() {
				return isOrderViewInListViewMode;
			}
			void set(bool val)
			{
				if (isOrderViewInListViewMode != val)
				{
					isOrderViewInListViewMode = val;
					RaisePropertyChanged(PROPERTYNAME(IsOrderviewInListViewMode));
				}
			}
		}

		property SalesOrderWrapper^ CurrentOrder
		{
			SalesOrderWrapper^ get() 
			{ 
				return current; 
			}

			void set(SalesOrderWrapper^ val)
			{
				if (current != val)
				{
					current = val;
					RaisePropertyChanged(PROPERTYNAME(CurrentOrder));
				}
			}
		}

		property IVector<OrderGroupedInfoList^>^ GroupedOrders
		{
			IVector<OrderGroupedInfoList^>^ get()
			{
				return orderGroups;
			}
			void set(IVector<OrderGroupedInfoList^>^ val)
			{
				if (orderGroups != val)
				{
					orderGroups = val;
					RaisePropertyChanged(PROPERTYNAME(GroupedOrders));
				}
			}
		}


		property IVector<SalesOrderWrapper^>^ Orders
		{
			IVector<SalesOrderWrapper^>^ get()
			{
				return orders;
			}
			void set(IVector<SalesOrderWrapper^>^ val)
			{
				if (orders != val)
				{
					orders = val;
					RaisePropertyChanged(PROPERTYNAME(Orders));
				}
			}
		}

		void UpdateCurrentOrder(SalesOrderWrapper^ orderwrapper);

	SalesOrderWrapper^ NewOrderWrapper();

	void RevertCurrentOrder();

	void SaveCurrentOrder();

	internal:
		/*task<void> */void PopulateOrders();

	private:
		DECLARERAISEPROPCHANGED;

	};

}