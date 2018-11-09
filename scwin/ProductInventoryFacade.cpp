#include "pch.h"
#include "ProductInventoryFacade.h"

DEFINERAISEPROPCHANGEDBASIC(ProductInventoryFacade)

ProductInventoryFacade::ProductInventoryFacade(Product^ product) :product(product)
{
	//TODO: setting a default value for now until inventory work is completed
	properties = ref new ProductInventoryProperties();
}

