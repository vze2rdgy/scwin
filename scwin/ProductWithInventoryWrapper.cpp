#include "pch.h"
#include "ProductWithInventoryWrapper.h"

DEFINERAISEPROPCHANGEDBASIC(ProductWithInventoryWrapper)

ProductWithInventoryWrapper::ProductWithInventoryWrapper(Product^ product) 
	: product(product)
{

}

PricePlanViewModelFacade^ ProductWithInventoryWrapper::PricePlan::get()
{
	return priceplan;
}

void ProductWithInventoryWrapper::PricePlan::set(PricePlanViewModelFacade^ val)
{
	if (priceplan != val)
	{
		priceplan = val;
		RaisePropertyChanged(PROPERTYNAME(PricePlan));
	}
}


void scwin::ProductWithInventoryWrapper::LoadPriceInfo()
{
	priceplan = ref new PricePlanViewModelFacade(this->product);
	OutputDebugString(priceplan->GetPricePlanType()->Data());
}

void scwin::ProductWithInventoryWrapper::LoadInventoryInfo()
{
	inventory = ref new ProductInventoryFacade(product);
}


IAsyncOperation<IVector<ProductWithInventoryWrapper^>^>^ scwin::ProductWithInventoryWrapper::BuildList(ProductChooseOptions options)
{
	return create_async([options]()->IVector<ProductWithInventoryWrapper^>^
	{
		IVector<ProductWithInventoryWrapper^>^ list = ref new Vector<ProductWithInventoryWrapper^>();
		auto lvs = scLocalDataService::Instance();
		auto prodList = lvs->GetProducts();
		switch (options)
		{
		case ProductChooseOptions::ProductInfoWithPrice:
			break;
		case ProductChooseOptions::ProductInfoWithPriceAndInventory:
			break;
		}
		return list;
	});
}


