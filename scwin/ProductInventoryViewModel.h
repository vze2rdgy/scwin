#pragma once

namespace scwin
{

	public ref class ProductInventoryViewModel sealed : INotifyPropertyChanged
	{
		IObservableVector<ProductInventoryLocation^>^ storeLocations;
		ProductInventoryLocation^ editedStorageLocation;
		bool bIsPaneOpen;
		IObservableVector<UserProfile^>^ internalUsers;
		bool hasLocationAddress;
		bool storeLocationLoaded;
		IObservableVector<ProductInventoryFacade^>^ productsInventories;

	public:
		ProductInventoryViewModel();

		void Initialize();

		void FetchProductInventories();

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property IObservableVector<ProductInventoryLocation^>^ StorageLocations
		{
			IObservableVector<ProductInventoryLocation^>^ get();
			void set(IObservableVector<ProductInventoryLocation^>^);
		}

		property IObservableVector<ProductInventoryFacade^>^ ProductsInventory
		{
			IObservableVector<ProductInventoryFacade^>^ get();
			void set(IObservableVector<ProductInventoryFacade^>^);
		}

		property bool StoreLocationLoaded
		{
			bool get();
		private:
			void set(bool);
		}

		property bool IsPaneOpen
		{
			bool get();
			void set(bool);
		}

		property Platform::Object^ SelectedItem;

		property ProductInventoryLocation^ EditedStorageLocation
		{
			ProductInventoryLocation^ get();
			void set(ProductInventoryLocation^);
		}

		property IObservableVector<UserProfile^>^ InternalUsers
		{
			IObservableVector<UserProfile^>^ get();
			void set(IObservableVector<UserProfile^>^ val);
		}

		property bool HasLocationAddress
		{
			bool get();
			void set(bool);
		}

		property int SelectedIndex;

		property UserProfile^ SelectedLocationContact;

		property Command^ SaveCommand;



	private:

		void ExecuteSaveCommand(Platform::Object^ parameter);
		bool CanExecuteSaveCommand(Platform::Object^ parameter);

		DECLARERAISEPROPCHANGED;

	};

}