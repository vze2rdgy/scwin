#include "pch.h"
#include "SalesOrderWrapper.h"
#include "cpplinq.hpp"

DEFINERAISEPROPCHANGEDBASIC(SalesOrderWrapper)
DEFINERAISEPROPCHANGEDBASIC(SalesOrderDetailsWrapper)


SalesOrderWrapper::SalesOrderWrapper(SalesOrder^ order)
{
	Order = order;
	FetchWrapperNeededData();
	Order->PropertyChanged += ref new PropertyChangedEventHandler(this, &SalesOrderWrapper::OnOrderPropertyChanged);
}

void scwin::SalesOrderWrapper::UpdateCostCenterAddress()
{
	BillingAddress = nullptr;
	ShippingAddress = nullptr;
	if (costcenter != nullptr)
	{
		create_task([=]()
		{
			auto lvs = scLocalDataService::Instance();
			if (!StringHelpers::IsNullOrEmpty(costcenter->BillingAddress))
				BillingAddress = lvs->GetAddress(costcenter->BillingAddress);
			if (!StringHelpers::IsNullOrEmpty(costcenter->ShippingAddress))
				ShippingAddress = lvs->GetAddress(costcenter->ShippingAddress);
		});
	}
}

void scwin::SalesOrderWrapper::OrderDetailsVectorChanged(IObservableVector<SalesOrderDetailsWrapper^>^ v, IVectorChangedEventArgs ^ args)
{
	if (args->CollectionChange == CollectionChange::ItemInserted)
	{
		auto newItem = v->GetAt(args->Index);
		newItem->PropertyChangedToken = newItem->PropertyChanged += ref new PropertyChangedEventHandler(this, &SalesOrderWrapper::OrderDetailItemPropertyChanged);
	}
	else if (args->CollectionChange == CollectionChange::ItemRemoved)
	{
		//auto removedItem = v->GetAt(args->Index);
		//removedItem->PropertyChanged -= removedItem->PropertyChangedToken;
		auto  linenet = 0.;
		for (auto item : v)
		{
			linenet += item->InnerItem->Net;
		}
		OrderTotalBeforeTaxesAndDiscounts = linenet;
	}

	UpdateOrderValidity();

}

void scwin::SalesOrderWrapper::OrderDetailItemPropertyChanged(Object ^ sender, PropertyChangedEventArgs ^ eargs)
{
	if (orderDetails == nullptr) return;

	if (eargs->PropertyName == "Net") // see order details wrapper ComputeOrderLineItemTotal method.
	{

		auto  linenet = 0.;
		for (auto item : orderDetails)
		{
			linenet += item->InnerItem->Net;
		}
		OrderTotalBeforeTaxesAndDiscounts = linenet;
	}

	UpdateOrderValidity();

}


IVector<SalesOrderStatusHistory^>^ SalesOrderWrapper::OrderStatusHistory::get()
{
	if (oshistory == nullptr || oshistory->Size == 0)
	{
		auto lvs = scLocalDataService::Instance();
		// no history except draft for new order
		if (order->ObjectState == DataState::Added)
		{
			if (oshistory == nullptr)
				oshistory = ref new Vector<SalesOrderStatusHistory^>();
			auto draftStatus = ref new SalesOrderStatusHistory();
			draftStatus->OrderId = order->Id;
			draftStatus->OrderStatus = this->OrderStatus;
			draftStatus->OrderStatusDate = order->DateAndTime;
			draftStatus->CreatedBy = lvs->GetCurrentUser()->Name;
			oshistory->Append(draftStatus);
		}
		else
		{
			oshistory = lvs->GetOrderStatusHistory(order->Id);
		}
	}
	return oshistory;
}


IVector<SalesOrderPaymentHistory^>^ SalesOrderWrapper::PaymentStatusHistory::get()
{
	if (opHistory == nullptr || opHistory->Size == 0)
	{
		auto lvs = scLocalDataService::Instance();
		// no history except draft for new order
		if (order->ObjectState == DataState::Added)
		{
			if (opHistory == nullptr)
				opHistory= ref new Vector<SalesOrderPaymentHistory^>();
			auto opStatus = ref new SalesOrderPaymentHistory();
			opStatus->PaymentStatus = PaymentStatus;
			opStatus->PayDetails = "Payment Information Not Provided";
			opHistory->Append(opStatus);
		}
		else
		{
			opHistory = lvs->GetOrderPaymentHistory(order->Id);
		}
	}
	return opHistory;
}


IVector<SalesOrderDetailsWrapper^>^ SalesOrderWrapper::OrderDetails::get()
{
	return orderDetails;
}

void SalesOrderWrapper::OrderDetails::set(IVector<SalesOrderDetailsWrapper^>^ val)
{
	if (orderDetails != val)
	{
		if (orderDetails != nullptr)
		{
			orderDetails->VectorChanged -= orderDetailsVectorChangedToken;
		}
		orderDetails = (Vector<SalesOrderDetailsWrapper^>^)val;
		orderDetailsVectorChangedToken = orderDetails->VectorChanged +=
			ref new VectorChangedEventHandler<SalesOrderDetailsWrapper^>(this, &SalesOrderWrapper::OrderDetailsVectorChanged);

		RaisePropertyChanged(PROPERTYNAME(OrderDetails));
	}
}

double SalesOrderWrapper::OrderTotalBeforeTaxesAndDiscounts::get()
{
	return order->GrossAmount;
}

void SalesOrderWrapper::OrderTotalBeforeTaxesAndDiscounts::set(double val)
{
	if (order->GrossAmount != val)
	{
		order->GrossAmount = val;
		RaisePropertyChanged(PROPERTYNAME(OrderTotalBeforeTaxesAndDiscounts));
		CalculateOrderTotals();
	}
}

DiscountTypes SalesOrderWrapper::OrderDiscountType::get()
{
	return order->DiscountType;
}

void SalesOrderWrapper::OrderDiscountType::set(DiscountTypes val)
{
	if (order->DiscountType != val)
	{
		order->DiscountType = val;
		RaisePropertyChanged(PROPERTYNAME(OrderDiscountType));
		CalculateOrderTotals();
	}
}

double SalesOrderWrapper::OrderDiscount::get()
{
	return order->Discount;
}

void SalesOrderWrapper::OrderDiscount::set(double val)
{
	if (order->Discount != val)
	{
		order->Discount = val;
		RaisePropertyChanged(PROPERTYNAME(OrderDiscount));
		CalculateOrderTotals();
	}
}


double SalesOrderWrapper::OrderPreTaxTotal::get()
{
	return orderPreTaxTotal;
}

void SalesOrderWrapper::OrderPreTaxTotal::set(double val)
{
	if (orderPreTaxTotal != val)
	{
		orderPreTaxTotal = val;
		RaisePropertyChanged(PROPERTYNAME(OrderPreTaxTotal));
	}
}

double SalesOrderWrapper::OrderTaxPct::get()
{
	return order->TaxPct;
}

void SalesOrderWrapper::OrderTaxPct::set(double val)
{
	if (order->TaxPct != val)
	{
		order->TaxPct = val;
		RaisePropertyChanged(PROPERTYNAME(OrderTaxPct));
		CalculateOrderTotals();
	}
}

double SalesOrderWrapper::OrderFinal::get()
{
	return order->FinalTotal;
}

void SalesOrderWrapper::OrderFinal::set(double val)
{
	if (order->FinalTotal!= val)
	{
		order->FinalTotal = val;
		RaisePropertyChanged(PROPERTYNAME(OrderFinal));
	}
}


void scwin::SalesOrderWrapper::CalculateOrderTotals()
{
	// calculations starts from order->GrossAmount
	// first discount

	if (order->DiscountType == DiscountTypes::Amount) // substract from gross
	{
		OrderPreTaxTotal = OrderTotalBeforeTaxesAndDiscounts - OrderDiscount;
	}
	else if (order->DiscountType == DiscountTypes::Percent)
	{
		if (OrderDiscount == 0)
			OrderPreTaxTotal = OrderTotalBeforeTaxesAndDiscounts;
		else if (OrderDiscount > 100.0)
		{
			PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "Discount amount cannot be larger than order total.");
			OrderPreTaxTotal = OrderTotalBeforeTaxesAndDiscounts;
			return;
		}
		else
			OrderPreTaxTotal = OrderTotalBeforeTaxesAndDiscounts * ( 1- OrderDiscount / 100.);
	}

	if (OrderTaxPct > 0.0 && OrderTaxPct <= 100.)
	{
		OrderFinal = OrderPreTaxTotal * (1  + OrderTaxPct / 100.0);
	}
	else if (OrderTaxPct == 0.00)
		OrderFinal = OrderPreTaxTotal ;
	else
	{
		PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "Tax percent entered is invalid.");
	}
	RaisePropertyChanged("OrderPreTaxTotal");
	RaisePropertyChanged("OrderFinal");
}

void scwin::SalesOrderWrapper::OnOrderPropertyChanged(Object ^ sender, PropertyChangedEventArgs ^ eargs)
{
	UpdateOrderValidity();
}

void scwin::SalesOrderWrapper::FetchWrapperNeededData()
{
	// get customer from costcenter
	if (order == nullptr) return;
	auto lvs = scLocalDataService::Instance();
	if (order->CostCenterId != nullptr)
	{
		customer = lvs->GetCustomerOrgOfCostCenter(order->CostCenterId);
		costcenter = lvs->GetCustomerCostCenter(order->CostCenterId);
		billingAddress = lvs->GetAddress(costcenter->BillingAddress);
		shippingAddress = lvs->GetAddress(costcenter->ShippingAddress);
	}

	auto os = lvs->GetLatestOrderStatus(order->Id);
	if (os == nullptr)
		orderStatus = lvs->GetGeneralStatus(ORDERSTATUS, 0);
	else
		orderStatus = os->OrderStatus;

	auto ps = lvs->GetLatestOrderPaymentStatus(order->Id);
	if (ps == nullptr)
		paymentStatus = lvs->GetGeneralStatus(PAYMENTSTATUS, 0);
	else
		paymentStatus = ps->PaymentStatus;
	
	if (order->ContactId != nullptr)
		contact = lvs->GetCustomerContact(order->ContactId);

	//OrderDiscountType

	create_task([=]()
	{
		using namespace cpplinq;
		// asynchronously populate order details
		auto baseOrderDets = lvs->GetSalesOrderDetailList(order->Id);
		// convert
		auto converteddets = from(to_vector(baseOrderDets)) >> select([](SaleOrderDetails^ det)
		{
			return ref new SalesOrderDetailsWrapper(det);
		}) >> to_vector();
		OrderDetails = ref new Vector<SalesOrderDetailsWrapper^>(converteddets);
	});
}

SalesOrder^ SalesOrderWrapper::Order::get()
{
	return order; 
}

void SalesOrderWrapper::Order::set(SalesOrder^ val)
{
	if (order != val)
	{
		order = val;
		RaisePropertyChanged(PROPERTYNAME(Order));
		RaisePropertyChanged(PROPERTYNAME(IsExistingOrder));
	}
}


bool SalesOrderWrapper::IsExistingOrder::get()
{
	return 
		order == nullptr ? false : 
		order->ObjectState == DataState::Edited || 
		order->ObjectState == DataState::Unmodified;
}

bool SalesOrderWrapper::HasValidChanges::get()
{
	return hasValidChanges;
}

bool SalesOrderWrapper::HasChanges::get()
{
	return hasChanges;
}

bool SalesOrderWrapper::CanRevert::get()
{
	return order == nullptr ? false : ((order->ObjectState == DataState::Added || order->ObjectState == DataState::Edited) && HasChanges);
}

void scwin::SalesOrderWrapper::ResetChangeStatus()
{

	// Update Order and details to unmodified state
	order->ObjectState = DataState::Unmodified;
	for (auto odet : orderDetails)
	{
		odet->InnerItem->ObjectState = DataState::Unmodified;
	}

	for (auto odet : orderDetails)
	{
		odet->InnerItem->ObjectState = DataState::Unmodified;
	}

	for (auto os : OrderStatusHistory)
	{
		os->ObjectState = DataState::Unmodified;
	}

	for (auto op : PaymentStatusHistory)
	{
		op->ObjectState = DataState::Unmodified;
	}

	hasChanges = false;
	hasValidChanges = false;

	RaisePropertyChanged(PROPERTYNAME(HasChanges));
	RaisePropertyChanged(PROPERTYNAME(HasValidChanges));
}

void scwin::SalesOrderWrapper::UpdateOrderValidity()
{

	hasChanges = true;
	RaisePropertyChanged(PROPERTYNAME(HasChanges));


	bool isValid = true;
	if (isValid)
	{
		if (billingAddress == nullptr || shippingAddress == nullptr)
		{
			isValid = false;
		}
	}


	if (isValid)
	{
		// check if there are order items
		if (orderDetails == nullptr)
			isValid = false;
		else if (orderDetails->Size == 0)
			isValid = false;
	}


		// check if any of orderdetails is invalid
	if (isValid)
	{
		for (auto odet : orderDetails)
		{
			if (!odet->InnerItem->IsValid)
			{
				isValid = false;
				break;
			}
		}
	}
	
	if (isValid)
		isValid = order->IsValid;

	hasValidChanges = isValid;

	OutputDebugStringA(HasValidChanges ? "Current Order is valid\n" : "Current Order is invalid.");

	//if (!hasValidChanges)
	//{
	//	__debugbreak();
	//}

	RaisePropertyChanged(PROPERTYNAME(HasValidChanges));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////


scwin::SalesOrderDetailsWrapper::SalesOrderDetailsWrapper(String^ salesOrderId)
	:orderdet(ref new SaleOrderDetails())
{
	InnerItem->OrderId = salesOrderId;
}

scwin::SalesOrderDetailsWrapper::SalesOrderDetailsWrapper(SaleOrderDetails ^ orderdet)
	:orderdet(orderdet)
{
	if (orderdet == nullptr)
		throw Exception::CreateException(E_INVALIDARG, "orderdet parameter must be valid instance.");

	LoadOrderDetailsItems();
}

ProductWithInventoryWrapper^ SalesOrderDetailsWrapper::ProductItem::get()
{
	return prodWrapper;
}

void SalesOrderDetailsWrapper::ProductItem::set(ProductWithInventoryWrapper^ val)
{
	if (prodWrapper != val)
	{
		prodWrapper = val;
		RaisePropertyChanged(PROPERTYNAME(ProductItem));
		//Update few local properties for the product
		if (prodWrapper != nullptr)
		{
			if (prodWrapper->PricePlan != nullptr)
			{
				IsTieredPricePlan = prodWrapper->PricePlan->IsTieredPricePlan;
				RaisePropertyChanged(PROPERTYNAME(IsTieredPricePlan));
			}
		}
	}
}

double SalesOrderDetailsWrapper::UnitPrice::get()
{
	return orderdet->UnitPrice;
}

void SalesOrderDetailsWrapper::UnitPrice::set(double val)
{
	if (val != orderdet->UnitPrice)
	{
		orderdet->UnitPrice = val;
		ComputeOrderLineItemTotal();
		RaisePropertyChanged(PROPERTYNAME(UnitPrice));
	}
}

uint64 SalesOrderDetailsWrapper::Quantity::get()
{
	return orderdet->Quantity;
}

void SalesOrderDetailsWrapper::Quantity::set(uint64 val)
{
	using namespace cpplinq;
	if (IsTieredPricePlan)
	{
		// if unit price is not set, find the correct unit price from the quanty
		// ensure the vector is sorted
		auto vtieredprices = to_vector(prodWrapper->PricePlan->PriceDetails);
		auto pricetier = from(vtieredprices) >> where([val](IPricePlanDetails^ priceItem)
		{
			auto tieredPlan = (PricePlanTierDetails^)priceItem;
			if (val >= tieredPlan->BeginQuantity && (val <= tieredPlan->EndQuantity || tieredPlan->EndQuantity == 0))
				return true;
			return false;
		}) >> first_or_default();
		if (pricetier == nullptr)
		{
			UnitPrice = 0.0; // order shouldn't be allowed to save
		}
		else
		{
			UnitPrice = pricetier->Price;
		}
	}
	else
	{
		if (UnitPrice == 0.0)
		{
			UnitPrice = prodWrapper->PricePlan->DefaultPrice->Price;
		}
	}

	orderdet->Quantity = val;


	//Update Gross and net
	ComputeOrderLineItemTotal();

	RaisePropertyChanged(PROPERTYNAME(Quantity));
}

void scwin::SalesOrderDetailsWrapper::ComputeOrderLineItemTotal()
{
	// set gross
	orderdet->Gross = UnitPrice * Quantity;
	if (orderdet->DiscountType == DiscountTypes::Amount) // substract from gross
	{
		if (orderdet->Discount > orderdet->Gross)
		{
			Message = "Discount amount cannot be larger than line total (See Gross Amount).";
			return;
		}
		orderdet->Net = orderdet->Gross - orderdet->Discount;
	}
	else if (orderdet->DiscountType == DiscountTypes::Percent)
	{
		if (orderdet->Discount == 0)
			orderdet->Net = orderdet->Gross;
		else if (orderdet->Discount > 100.0)
		{
			Message = "Discount amount cannot be larger than line total (See Gross Amount).";
			orderdet->Net = 0.;
			return;
		}
		else
			orderdet->Net = orderdet->Gross * (1 - orderdet->Discount / 100.);
	}

	RaisePropertyChanged("Net");
}

void scwin::SalesOrderDetailsWrapper::LoadOrderDetailsItems()
{
	if (orderdet != nullptr)
	{
		auto lvs = scLocalDataService::Instance();
		auto prod = lvs->GetProduct(orderdet->ProductId);
		if (prod != nullptr)
		{
			prodName = prod->Name;
		 	ProductItem = ref new ProductWithInventoryWrapper(prod);
		}
	}
}

double SalesOrderDetailsWrapper::Discount::get()
{
	return orderdet->Discount;
}

void SalesOrderDetailsWrapper::Discount::set(double val)
{
	try
	{
		if (orderdet->Discount != val)
		{
			orderdet->Discount = val;
			RaisePropertyChanged(PROPERTYNAME(Discount));
			ComputeOrderLineItemTotal();
		}
	}
	catch (Exception^ ex)
	{
		Message = ex->Message;
	}
}


DiscountTypes SalesOrderDetailsWrapper::DiscountType::get()
{
	return orderdet->DiscountType;
}

void SalesOrderDetailsWrapper::DiscountType::set(DiscountTypes val)
{
	try
	{
		if (orderdet->DiscountType != val)
		{
			orderdet->DiscountType = val;
			RaisePropertyChanged(PROPERTYNAME(DiscountType));
			ComputeOrderLineItemTotal();
		}
	}
	catch (Exception^ ex)
	{
		Message = ex->Message;
	}
}

