#include "pch.h"
#include "SaleOrderViewModel.h"
#include "cpplinq.hpp"

/*
<MenuFlyoutItem x:Name="mnuOrderByStatus" Text="Order Status" Tapped="mnuOrderByStatus_Tapped"/>
<MenuFlyoutItem x:Name="mnuOrderByCustomer" Text="Customer" Tapped="mnuOrderByCustomer_Tapped"/>
<MenuFlyoutItem x:Name="mnuOrderByAmount" Text="Total" Tapped="mnuOrderByAmount_Tapped"/>
<MenuFlyoutItem x:Name="mnuOrderByDate" Text="Date" Tapped="mnuOrderByDate_Tapped"/>

*/


SaleOrderViewModel::SaleOrderViewModel()
{
	orderGroups = ref new Vector<OrderGroupedInfoList^>();

	sortSelections = ref new Vector<String^>({ "Order Status", "Customer", "Total", "Date" });
	auto prop = scLocalDataService::Instance()->GetProperty("OrdersViewType");
	if (prop != nullptr)
	{
		this->IsOrderviewInListViewMode = prop->Value == "1";
	}
	prop = scLocalDataService::Instance()->GetProperty("OrderSorts");
	if (prop != nullptr && prop->HasValue())
	{
		this->CurrentSortSelection = prop->Value;
	}
	else
	{
		CurrentSortSelection = sortSelections->First()->Current;
	}

	OrderStatusList = scLocalDataService::Instance()->GetGeneralStatus(ORDERSTATUS);
	PaymentStatusList = scLocalDataService::Instance()->GetGeneralStatus(PAYMENTSTATUS);

	LoadState();
}

void scwin::SaleOrderViewModel::LoadState()
{
	IsLoaded = true;
}

void scwin::SaleOrderViewModel::SaveState()
{
	auto prop = ref new AppProperty("OrdersViewType", IsOrderviewInListViewMode ? "1" : "0");
	prop->IsRoaming = true;
	scLocalDataService::Instance()->SetProperty(prop);
	prop = ref new AppProperty("OrderSorts", CurrentSortSelection);
	prop->IsRoaming = true;
	scLocalDataService::Instance()->SetProperty(prop);
}

/*task<void>*/ void scwin::SaleOrderViewModel::PopulateOrders()
{
	WeakReference wr(this);
	//return create_task([wr]()
	//{
	auto ths = wr.Resolve<SaleOrderViewModel>();

	if (ths == nullptr) return;

	using namespace cpplinq;
	// if no orders already populated, get all orders
	if (ths->baseOrders == nullptr)
	{
		ths->baseOrders = scLocalDataService::Instance()->GetSalesOrders(nullptr, nullptr);
	}
	// wrap them and sort them
	auto vorders = to_vector(ths->baseOrders);
	auto wrapped = from(vorders) >> select([](SalesOrder^ order)
	{
		return ref new SalesOrderWrapper(order);
	});
	// now sort
	if (StringHelpers::IsNullOrEmpty(ths->currentSortSelection) || ths->currentSortSelection == "Order Status")
	{
		ths->Orders = ref new Vector<SalesOrderWrapper^>(wrapped >> orderby([](SalesOrderWrapper^ wrapper)
		{
			return wrapper->OrderStatus->PublicName;
		}) >> to_vector()
			);
	}
	else if (ths->currentSortSelection == "Customer")
	{
		ths->Orders = ref new Vector<SalesOrderWrapper^>(wrapped >> orderby([](SalesOrderWrapper^ wrapper)
		{
			return wrapper->Customer->Name;
		}) >> to_vector()
			);
	}
	else if (ths->currentSortSelection == "Total")
	{
		ths->Orders = ref new Vector<SalesOrderWrapper^>(wrapped >> orderby([](SalesOrderWrapper^ wrapper)
		{
			return wrapper->OrderFinal;
		}) >> to_vector()
			);
	}
	else if (ths->currentSortSelection == "Date")
	{
		ths->Orders = ref new Vector<SalesOrderWrapper^>(wrapped >> orderby([](SalesOrderWrapper^ wrapper)
		{
			return wrapper->Order->DateAndTime->UnixTime;
		}) >> to_vector()
			);

	}

	// create a grouped list
	// since above conditions already ordered the items, grouping can be smoother
	// cpplinq currently doesn't support grouping yet.
	if (StringHelpers::IsNullOrEmpty(ths->currentSortSelection) || ths->currentSortSelection == "Order Status")
	{
		auto map = ref new Map<int, OrderGroupedInfoList^>();
		for (auto ow : ths->orders)
		{
			OrderGroupedInfoList^ groupedList;
			if (!map->HasKey(ow->OrderStatus->Id))
			{
				map->Insert(ow->OrderStatus->Id, groupedList = ref new OrderGroupedInfoList());
				groupedList->Key = ow->OrderStatus->PrivateName;
				groupedList->Orders = ref new Vector<SalesOrderWrapper^>();
			}
			else
			{
				groupedList = map->Lookup(ow->OrderStatus->Id);
			}
			groupedList->Orders->Append(ow);
		}
		if (AppDispatcher->HasThreadAccess)
		{
			for (auto kvp : map)
				ths->orderGroups->Append(kvp->Value);
		}
		else
		{
			AppDispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([ths, map]()
			{
				for (auto kvp : map)
					ths->orderGroups->Append(kvp->Value);
			}));
		}
	}
	else if (StringHelpers::IsNullOrEmpty(ths->currentSortSelection) || ths->currentSortSelection == "Customer")
	{
		auto map = ref new Map<String^, OrderGroupedInfoList^>();
		for (auto ow : ths->orders)
		{
			OrderGroupedInfoList^ groupedList;
			if (!map->HasKey(ow->Customer->Id))
			{
				map->Insert(ow->Customer->Id, groupedList = ref new OrderGroupedInfoList());
				groupedList->Key = ow->Customer->Name;
				groupedList->Orders = ref new Vector<SalesOrderWrapper^>();
			}
			else
			{
				groupedList = map->Lookup(ow->Customer->Id);
			}
			groupedList->Orders->Append(ow);
		}
		for (auto kvp : map)
			ths->orderGroups->Append(kvp->Value);
	}
	else if (StringHelpers::IsNullOrEmpty(ths->currentSortSelection) || ths->currentSortSelection == "Total")
	{
		auto map = ref new Map<float64, OrderGroupedInfoList^>();
		for (auto ow : ths->orders)
		{
			OrderGroupedInfoList^ groupedList;
			if (!map->HasKey(ow->OrderFinal))
			{
				map->Insert(ow->OrderFinal, groupedList = ref new OrderGroupedInfoList());
				groupedList->Key = StringHelpers::ConvertToString(std::to_string(ow->OrderFinal).c_str());
				groupedList->Orders = ref new Vector<SalesOrderWrapper^>();
			}
			else
			{
				groupedList = map->Lookup(ow->OrderFinal);
			}
			groupedList->Orders->Append(ow);
		}
		for (auto kvp : map)
			ths->orderGroups->Append(kvp->Value);
	}
	else if (StringHelpers::IsNullOrEmpty(ths->currentSortSelection) || ths->currentSortSelection == "Date")
	{
		auto map = ref new Map<int, OrderGroupedInfoList^>();
		for (auto ow : ths->orders)
		{
			OrderGroupedInfoList^ groupedList;
			if (!map->HasKey(ow->Order->DateAndTime->ShortDateAsNumber))
			{
				map->Insert(ow->Order->DateAndTime->ShortDateAsNumber, groupedList = ref new OrderGroupedInfoList());
				groupedList->Key = ow->Order->DateAndTime->ToShortFormatString();
				groupedList->Orders = ref new Vector<SalesOrderWrapper^>();
			}
			else
			{
				groupedList = map->Lookup(ow->Order->DateAndTime->ShortDateAsNumber);
			}
			groupedList->Orders->Append(ow);
		}
		for (auto kvp : map)
			ths->orderGroups->Append(kvp->Value);
	}
	//});
}

void scwin::SaleOrderViewModel::UpdateCurrentOrder(SalesOrderWrapper ^ orderwrapper)
{
	current = orderwrapper;
}

SalesOrderWrapper ^ scwin::SaleOrderViewModel::NewOrderWrapper()
{
	using namespace cpplinq;
	auto o = ref new SalesOrder();
	o->OrderNumber = SalesOrder::GenerateNewOrderNumber();
	auto wrapper = ref new SalesOrderWrapper(o);
	auto vOSList = to_vector(OrderStatusList);
	GeneralStatus^ draftStatus =
		from(vOSList) >>
	where([](GeneralStatus^ status)
	{
		return status->Id == 1;
	}) >>
		first_or_default();
	wrapper->OrderStatus = draftStatus;
	auto vPSList = to_vector(PaymentStatusList);
	GeneralStatus^ paymentStatus = from(vPSList) >>
	where([](GeneralStatus^ status)
	{
		return status->Id == 1;
	}) >>
		first_or_default();
	wrapper->PaymentStatus = paymentStatus;

	wrapper->OrderDetails = ref new Vector<SalesOrderDetailsWrapper^>();
	wrapper->OrderDetails->Append(ref new SalesOrderDetailsWrapper(wrapper->Order->Id));

	auto cal = ref new Windows::Globalization::Calendar();
	cal->AddMonths(1);
	wrapper->Order->CustomerExpectedDate = Date::ConvertFromUWPDateTime(cal->GetDateTime());

	wrapper->Order->RepId = scLocalDataService::Instance()->GetCurrentUser()->Name;

	wrapper->OrderStatusHistory->First()->Current->OrderStatusDate = wrapper->Order->DateAndTime;

	return wrapper;
}

void scwin::SaleOrderViewModel::RevertCurrentOrder()
{
	using namespace cpplinq;

	if (!CurrentOrder->CanRevert) return;

	// if the order is new, clear all properties that are set except order number
	if (CurrentOrder->Order->ObjectState == DataState::Added)
	{
		auto orderDetails = ref new Vector<SalesOrderDetailsWrapper^>();
		orderDetails->Append(ref new SalesOrderDetailsWrapper(CurrentOrder->Order->Id));
		CurrentOrder->OrderDetails = orderDetails;
		// remote customer info

		//  first clear customer vm selections (choose customer screen)
		// find cost center and contact id

		auto vOrgs = to_vector(CustomerViewmodel->Organizations);
		if (CurrentOrder->Customer != nullptr)
		{
			auto org = from(vOrgs) >> where([=](CustomerDtosGroupedBindable^ customer)
			{
				if (customer->Organization->Id == CurrentOrder->Customer->Id)
				{
					return true;
				}
			}) >> first_or_default();
			if (org != nullptr && CurrentOrder->CostCenter != nullptr)
			{
				auto vCostCenters = to_vector(org->CostCenters);
				auto cc = from(vCostCenters) >> where([=](CostCenterDtosGroupedBindable^ costcenter)
				{
					if (costcenter->CostCenter->Id == CurrentOrder->CostCenter->Id)
					{
						return true;
					}
				}) >> first_or_default();
				if (cc != nullptr)
				{
					cc->IsCostCenterSelectedForSalesOrder = false;
					if (CurrentOrder->Contact != nullptr)
					{
						auto vContacts = to_vector(cc->Contacts);
						auto c = from(vContacts) >> where([=](CustomerContactKeyValuePair^ kvp)
						{
							return kvp->Value->Id == CurrentOrder->Contact->Id;
						}) >> first_or_default();
						if (c != nullptr)
						{
							c->IsContactSelectedForSalesOrder = false;
						}
					}
				}
			}
		}

		CurrentOrder->Customer = nullptr;
		CurrentOrder->Contact = nullptr;
		CurrentOrder->CostCenter = nullptr;

		CurrentOrder->OrderStatusHistory->Clear();

		auto vOSList = to_vector(OrderStatusList);
		GeneralStatus^ draftStatus =
			from(vOSList) >>
		where([](GeneralStatus^ status)
		{
			return status->Id == 1;
		}) >>
			first_or_default();
		CurrentOrder->OrderStatus = draftStatus;
		auto vPSList = to_vector(PaymentStatusList);
		GeneralStatus^ paymentStatus = from(vPSList) >>
		where([](GeneralStatus^ status)
		{
			return status->Id == 1;
		}) >>
			first_or_default();
		CurrentOrder->PaymentStatus = paymentStatus;

		// keep the order status history
		CurrentOrder->OrderDiscountType = DiscountTypes::Percent;
		CurrentOrder->OrderPreTaxTotal = 0.;
		CurrentOrder->OrderFinal = 0.;
		CurrentOrder->OrderTaxPct = 0.;
		CurrentOrder->OrderDiscount = 0.;
		CurrentOrder->OrderTotalBeforeTaxesAndDiscounts = 0.;

		CurrentOrder->Order->DateAndTime = Date::Now;
		auto cal = ref new Calendar();
		cal->AddMonths(1);
		CurrentOrder->Order->CustomerExpectedDate = Date::ConvertFromUWPDateTime(cal->GetDateTime());
		CurrentOrder->Order->RepId = scLocalDataService::Instance()->GetCurrentUser()->Name;


	}
}

void scwin::SaleOrderViewModel::SaveCurrentOrder()
{
	// organize order, order details list, and status
	// check again validity
	if (!CurrentOrder->Order->IsValid)
	{
		throw Exception::CreateException(E_ABORT, "Order header is not valid. Please ensure that all required information such as customer, billing and shipping addresses are provided.");
	}
	for (auto odet : CurrentOrder->OrderDetails)
	{
		if (!odet->InnerItem->IsValid)
		{
			throw Exception::CreateException(E_ABORT, "One of the order line item is invalid. Please provide all required information.");
		}
	}

	// first save order
	auto lvs = scLocalDataService::Instance();
	// begin stransaction
	try
	{

		using namespace cpplinq;
		auto orderDets = ref new Vector<SaleOrderDetails^>(from(to_vector(CurrentOrder->OrderDetails)) >> select([](SalesOrderDetailsWrapper^ wrap)
		{
			return wrap->InnerItem;
		}) >> to_vector());
		auto currentOrderStatus = CurrentOrder->OrderStatusHistory->GetAt(CurrentOrder->OrderStatusHistory->Size - 1);
		auto currentPayStatus = CurrentOrder->PaymentStatusHistory->GetAt(CurrentOrder->PaymentStatusHistory->Size - 1);

		currentPayStatus->PayAmount = 0.;
		currentPayStatus->PaymentInstrument = from(to_vector(lvs->GetGeneralStatus(PAYMENTINSTRUMENT))) >> where([](GeneralStatus^ pi)
		{
			return pi->Id == 0;
		}) >> first_or_default();

		currentPayStatus->PaymentStatus = CurrentOrder->PaymentStatus;
		currentPayStatus->PaymentDate = CurrentOrder->Order->DateAndTime;

		auto msgId = lvs->SaveSalesOrder(CurrentOrder->Order, orderDets, currentOrderStatus, currentPayStatus);
		if (HASFAILED(msgId))
		{
			throw Exception::CreateException(E_FAIL, ref new String(GetMessage(msgId)));
		}

		CurrentOrder->ResetChangeStatus();

	}
	catch (...)
	{
		lvs->CancelTransaction();
		throw Exception::CreateException(E_ABORT, "Failed to save the order due to a unexpected exception. Please retry.");
	}
}

IVector<GeneralStatus^>^ SaleOrderViewModel::OrderStatusList::get()
{
	return orderStatusList;
}

void SaleOrderViewModel::OrderStatusList::set(IVector<GeneralStatus^>^ val)
{
	if (orderStatusList != val)
	{
		orderStatusList = val;
		RaisePropertyChanged(PROPERTYNAME(OrderStatusList));
	}
}


IVector<GeneralStatus^>^ SaleOrderViewModel::PaymentStatusList::get()
{
	return paymentStatusList;
}

void SaleOrderViewModel::PaymentStatusList::set(IVector<GeneralStatus^>^ val)
{
	if (paymentStatusList != val)
	{
		paymentStatusList = val;
		RaisePropertyChanged(PROPERTYNAME(PaymentStatusList));
	}
}


DEFINERAISEPROPCHANGEDBASIC(SaleOrderViewModel)