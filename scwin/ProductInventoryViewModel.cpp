#include "pch.h"
#include "ProductInventoryViewModel.h"
#include "cpplinq.hpp"


ProductInventoryViewModel::ProductInventoryViewModel()
{
	StoreLocationLoaded = false;
	StorageLocations = ref new Vector<ProductInventoryLocation^>();
	SaveCommand = ref new Command(
		ref new ExecuteAction(this, &ProductInventoryViewModel::ExecuteSaveCommand),
		ref new CanExecuteFunc(this, &ProductInventoryViewModel::CanExecuteSaveCommand)
	);

	Initialize();
}

void scwin::ProductInventoryViewModel::Initialize()
{
	auto service = scLocalDataService::Instance();
	using namespace cpplinq;

	WeakReference wr(this);
	task<IUserProfileList^> getUsers = create_task(service->ListUsers());
	getUsers.then([wr](IUserProfileList^ users)
	{
		// remove all customers
		auto employeesOnly = ref new Vector<UserProfile^>();
		auto nonSelection = ref new UserProfile();
		nonSelection->FullName = "";
		employeesOnly->Append(nonSelection);
		for (auto u : users)
		{
			if (from(to_vector(u->Roles)) >> any([](String^ roleName) { return roleName != "customer"; }))
				employeesOnly->Append(u);
		}
		wr.Resolve<ProductInventoryViewModel>()->InternalUsers = employeesOnly;
	});

	task<IObservableVector<ProductInventoryLocation^>^> tgetInvLocations =
		create_task(service->GetInventoryLocationsAsync());
	tgetInvLocations.then([wr](IObservableVector<ProductInventoryLocation^>^ list)
	{
		auto ths = wr.Resolve<ProductInventoryViewModel>();
		ths->StorageLocations = list;
		ths->StoreLocationLoaded = true;
	});
}

void scwin::ProductInventoryViewModel::FetchProductInventories()
{
	if (productsInventories == nullptr ||
		productsInventories->Size == 0)
	{
		// TODO 
	}
}

bool ProductInventoryViewModel::StoreLocationLoaded::get()
{
	return storeLocationLoaded;
}

void ProductInventoryViewModel::StoreLocationLoaded::set(bool val)
{
	storeLocationLoaded = val;
	RaisePropertyChanged(PROPERTYNAME(StoreLocationLoaded));
}

void scwin::ProductInventoryViewModel::ExecuteSaveCommand(Platform::Object ^ parameter)
{
	assert(parameter != nullptr);
	// check if it is storage location
	ProductInventoryLocation^ inventoryLocation = dynamic_cast<ProductInventoryLocation^>(parameter);
	if (inventoryLocation != nullptr)
	{
		// validate we only need location name
		if (!inventoryLocation->IsValid)
		{
			PubSubService::Instance->PublishError(this, "Failed to save. Please enter required information ('Location Name')");
			return;
		}

		if (HasLocationAddress)
		{
			if (nullptr == inventoryLocation->Address
				|| !inventoryLocation->Address->IsValid)
			{
				PubSubService::Instance->PublishError(this, "Failed to save. Address for the location is not provided. If address is not required, then uncheck 'Has Location Address' checkbox.");
				return;
			}
		}

		// contact is selected set it
		if (SelectedLocationContact != nullptr && !StringHelpers::IsNullOrEmpty(SelectedLocationContact->FullName))
		{
			inventoryLocation->ContactId = SelectedLocationContact->Id;
		}

		auto msgId = scLocalDataService::Instance()->SaveInventoryLocation(inventoryLocation);
		if (HASFAILED(msgId))
		{
			PubSubService::Instance->PublishError(this, ref new String(GetMessage(msgId)));
			return;
		}
		if (inventoryLocation->ObjectState == DataState::Added)
		{
			this->StorageLocations->Append(inventoryLocation);
			inventoryLocation->ObjectState = DataState::Unmodified;
		}
		IsPaneOpen = false;
		return;
	}
}

bool scwin::ProductInventoryViewModel::CanExecuteSaveCommand(Platform::Object ^ parameter)
{
	return true;
}

IObservableVector<ProductInventoryLocation^>^ ProductInventoryViewModel::StorageLocations::get()
{
	return storeLocations;
}

void ProductInventoryViewModel::StorageLocations::set(IObservableVector<ProductInventoryLocation^>^ val)
{
	if (storeLocations != val)
	{
		storeLocations = val;
		RaisePropertyChanged(PROPERTYNAME(StoreLocations));
	}
}

IObservableVector<ProductInventoryFacade^>^ ProductInventoryViewModel::ProductsInventory::get()
{
	return productsInventories;
}

void ProductInventoryViewModel::ProductsInventory::set(IObservableVector<ProductInventoryFacade^>^ val)
{
	if (productsInventories != nullptr)
	{
		productsInventories = val;
		RaisePropertyChanged(PROPERTYNAME(ProductsInventory));
	}
}

bool ProductInventoryViewModel::IsPaneOpen::get()
{
	return bIsPaneOpen;
}

void ProductInventoryViewModel::IsPaneOpen::set(bool val)
{
	if (bIsPaneOpen != val)
	{
		bIsPaneOpen = val;
		RaisePropertyChanged(PROPERTYNAME(IsPaneOpen));
	}
}

ProductInventoryLocation^ ProductInventoryViewModel::EditedStorageLocation::get()
{
	return editedStorageLocation;
}

void ProductInventoryViewModel::EditedStorageLocation::set(ProductInventoryLocation^ val)
{
	if (editedStorageLocation != val)
	{
		editedStorageLocation = val;
		RaisePropertyChanged(PROPERTYNAME(EditedStorageLocation));
		HasLocationAddress = editedStorageLocation != nullptr && editedStorageLocation->Address != nullptr;
	}
}

IObservableVector<UserProfile^>^ ProductInventoryViewModel::InternalUsers::get()
{
	return internalUsers;
}

void ProductInventoryViewModel::InternalUsers::set(IObservableVector<UserProfile^>^ val)
{
	if (internalUsers != val)
	{
		internalUsers = val;
		RaisePropertyChanged(PROPERTYNAME(InternalUsers));
	}
}

bool ProductInventoryViewModel::HasLocationAddress::get()
{
	return hasLocationAddress;
}

void ProductInventoryViewModel::HasLocationAddress::set(bool val)
{
	if (hasLocationAddress != val)
	{
		hasLocationAddress = val;
		RaisePropertyChanged(PROPERTYNAME(HasLocationAddress));
	}
}

DEFINERAISEPROPCHANGEDBASIC(ProductInventoryViewModel)