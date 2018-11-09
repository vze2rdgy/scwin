#pragma once

using namespace Windows::Foundation::Collections;


#include "StatusMessage.h"
#include "GeneralStatus.h"
#include "OrientationStateTrigger.h"
#include "ProductCategory.h"
#include "ProductPricePlan.h"
#include "PricePlanTierDetails.h"
#include "PricePlanStandardDetails.h"
#include "Product.h"
#include "ProductAttribute.h"
#include "IProductAttributeValue.h"
#include "GeneralAttributeValue.h"
#include "AttributeViewModelFacade.h"
#include "Address.h"
#include "CustomerAssignedSalesReps.h"
#include "CustomerContact.h"
#include "CustomerOrg.h"
#include "COrgCostCenter.h"
#include "FieldRep.h"
#include "CustomerDtosGroupedBindable.h"
#include "StringList.h"
#include "SalesOrderDtos.h"
#include "ProductInventory.h"
#include "ProductInventoryLocation.h"
#include "ProductInventoryTransaction.h"
#include "ProductInventoryProperties.h"
#include "AppBusinessOrg.h"

namespace scwin
{

	typedef IVector<IVectorView<ProductCategory^>^> ProductCategoryPath;
	typedef IVector<PricePlanTierDetails^> ProductTierPricePlans;
	typedef IVector<ProductPricePlan^> ProductHistoricalPricePlans;
	typedef IVector<IPricePlanDetails^> GenericPriceDetailsList;
	typedef IVector<ProductInventoryTransaction^> InventoryTransactions;
}
