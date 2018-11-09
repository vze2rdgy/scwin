#include "pch.h"
#include "SalesOrderDtos.h"

///////////////// SalesOrder ///////////////

DEFINERAISEPROPCHANGED(SalesOrder)
DEFINERAISEPROPCHANGED(SaleOrderDetails)

SalesOrder::SalesOrder()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	DateAndTime = Date::Now;
	CustomerExpectedDate = nullptr;
	CanRaiseEvents = true;
}

scwin::SalesOrder::SalesOrder(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

String ^ scwin::SalesOrder::GenerateNewOrderNumber()
{
	return scLocalDataService::Instance()->AutoGenerateCode(SALESORDERS);
}

double SalesOrder::TaxPct::get()
{
	return taxpct;
}

void SalesOrder::TaxPct::set(double val)
{
	if (taxpct != val)
	{
		taxpct = val;
		RaisePropertyChanged(PROPERTYNAME(TaxPct));
	}
}

double SalesOrder::Discount::get()
{
	return discNum;
}

void SalesOrder::Discount::set(double val)
{
	if (discType == DiscountTypes::Percent)
	{
		if (val > 100.0)
		{
			discNum = 100.0;
		}
		else if (val < 0.0)
		{
			discNum = 0.0;
		}
		else if (val >= 0.0 && val <= 1.0)
		{
			discNum = val * 100.0;
		}
		else
			discNum = val;
		RaisePropertyChanged(PROPERTYNAME(Discount));
	}
	else
	{
		if (val > gross)
		{
			throw Exception::CreateException(E_INVALIDARG, "Discount amount cannot be greater than order total.");
		}
		discNum = val;
		RaisePropertyChanged(PROPERTYNAME(Discount));
	}
}

bool SalesOrder::IsEmpty::get() { return false; }
bool SalesOrder::IsValid::get() 
{
	if (StringHelpers::IsNullOrEmpty(onumber))
		return false;
	if (StringHelpers::IsNullOrEmpty(ccorgccid))
		return false;
	if (!odatetime->IsValid)
		return false;
	if (gross == 0.)
		return false;
	if (net == 0.)
		return false;
	return true; 
}


/////////////////////// SaleOrderDetails ////////////////////////

scwin::SaleOrderDetails::SaleOrderDetails()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::SaleOrderDetails::SaleOrderDetails(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}



bool SaleOrderDetails::IsEmpty::get() { return false; }

bool SaleOrderDetails::IsValid::get() 
{ 
	if (StringHelpers::IsNullOrEmpty(soid))
		return false;
	if (StringHelpers::IsNullOrEmpty(pid))
		return false;
	if (pqty == 0)
		return false;

	// a sales may happen with unit price = 0 in some cases (free product)
	// which can also be done by setting discount amount = unit price
	if (unitprice == 0.0)
		return false;
	return true; 
}

double SaleOrderDetails::Discount::get()
{
	return pdisc;
}

void SaleOrderDetails::Discount::set(double val)
{
	if (discType == DiscountTypes::Percent)
	{
		if (val > 100.0)
		{
			pdisc = 100.0;
		}
		else if (val < 0.0)
		{
			pdisc = 0.0;
		}
		else if (val >= 0.0 && val <= 1.0)
		{
			pdisc = val * 100.0;
		}
		else
			pdisc = val;
		RaisePropertyChanged(PROPERTYNAME(Discount));
	}
	else
	{
		if (val > gross)
		{
			throw Exception::CreateException(E_INVALIDARG, "Discount amount cannot be greater than order line total.");
		}
		pdisc = val;
		RaisePropertyChanged(PROPERTYNAME(Discount));
	}
}



//////////////////////SalesOrderStatusHistory/////////////

scwin::SalesOrderStatusHistory::SalesOrderStatusHistory()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::SalesOrderStatusHistory::SalesOrderStatusHistory(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

bool SalesOrderStatusHistory::IsEmpty::get() { return false; }
bool SalesOrderStatusHistory::IsValid::get() { return true; }

DEFINERAISEPROPCHANGED(SalesOrderStatusHistory)


//////////////////////SalesOrderItemStatusHistory/////////////

scwin::SalesOrderItemStatusHistory::SalesOrderItemStatusHistory()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::SalesOrderItemStatusHistory::SalesOrderItemStatusHistory(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

bool SalesOrderItemStatusHistory::IsEmpty::get() { return false; }
bool SalesOrderItemStatusHistory::IsValid::get() { return true; }

DEFINERAISEPROPCHANGED(SalesOrderItemStatusHistory)



/////////////////// SalesOrderPaymentHistory /////////////////////////////////////

scwin::SalesOrderPaymentHistory::SalesOrderPaymentHistory()
{
	id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

scwin::SalesOrderPaymentHistory::SalesOrderPaymentHistory(String ^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

bool SalesOrderPaymentHistory::IsEmpty::get() { return false; }
bool SalesOrderPaymentHistory::IsValid::get() { return true; }

DEFINERAISEPROPCHANGED(SalesOrderPaymentHistory)

