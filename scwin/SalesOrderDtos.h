#pragma once

#include "dto.h"
#include "Date.h"
#include "GeneralStatus.h"

namespace scwin
{
	public ref class SalesOrder sealed : IDTOBase
	{
		String^ id;
		String^ onumber;
		String^ ccorgccid;
		String^ batchNo;
		Date^ odatetime;
		String^ cccontactid;
		String^ repid;
		double gross;
		String^ discCode;
		double discNum;
		DiscountTypes discType = DiscountTypes::Percent;
		String^ currencyid;
		double taxpct;
		double net;
		String^ notes;
		Date^ customerExpectedDate;
		String^ owner;

	public:
		SalesOrder();
		SalesOrder(String^ id);

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty {bool get(); }
		virtual property bool IsValid {bool get(); }
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ OrderNumber
		{
			String^ get()
			{
				return onumber;
			}
			void set(String^ val)
			{
				if (onumber != val)
				{
					onumber = val;
					RaisePropertyChanged(PROPERTYNAME(OrderNumber));
				}
			}
		}

		property String^ CostCenterId
		{
			String^ get()
			{
				return ccorgccid;
			}
			void set(String^ val)
			{
				if (ccorgccid != val)
				{
					ccorgccid = val;
					RaisePropertyChanged(PROPERTYNAME(CostCenterId));
				}
			}
		}

		property String^ BatchNumber
		{
			String^ get()
			{
				return batchNo;
			}
			void set(String^ val)
			{
				if (batchNo != val)
				{
					batchNo = val;
					RaisePropertyChanged(PROPERTYNAME(BatchNumber));
				}
			}
		}

		property Date^ DateAndTime
		{
			Date^ get()
			{
				return odatetime;
			}
			void set(Date^ val)
			{
				if (odatetime != val)
				{
					odatetime = val;
					RaisePropertyChanged(PROPERTYNAME(DateAndTime));
				}
			}
		}

		property String^ ContactId
		{
			String^ get()
			{
				return cccontactid;
			}
			void set(String^ val)
			{
				if (cccontactid != val)
				{
					cccontactid = val;
					RaisePropertyChanged(PROPERTYNAME(ContactId));
				}
			}
		}

		property String^ RepId
		{
			String^ get()
			{
				return repid;
			}
			void set(String^ val)
			{
				if (repid != val)
				{
					repid = val;
					RaisePropertyChanged("RepId");
				}
			}
		}

		property double GrossAmount
		{
			double get()
			{
				return gross;
			}
			void set(double val)
			{
				if (gross != val)
				{
					gross = val;
					RaisePropertyChanged(PROPERTYNAME(GrossAmount));
				}
			}
		}

		property String^ DiscountCode
		{
			String^ get()
			{
				return discCode;
			}
			void set(String^ val)
			{
				if (discCode != val)
				{
					discCode = val;
					RaisePropertyChanged(PROPERTYNAME(DiscountCode));
				}
			}
		}

		property double Discount
		{
			double get();
			void set(double);
		}

		property DiscountTypes DiscountType
		{
			DiscountTypes get()
			{
				return discType;
			}

			void set(DiscountTypes val)
			{
				if (discType != val)
				{
					discType = val;
					RaisePropertyChanged(PROPERTYNAME(DiscountType));
				}
			}
		}

		property String^ CurrencyId
		{
			String^ get()
			{
				return currencyid;
			}
			void set(String^ val)
			{
				if (currencyid != val)
				{
					currencyid = val;
					RaisePropertyChanged("CurrencyId");
				}
			}
		}

		property double TaxPct
		{
			double get();
			void set(double);
		}

		property double FinalTotal
		{
			double get()
			{
				return net;
			}
			void set(double val)
			{
				if (net != val)
				{
					net = val;
					RaisePropertyChanged(PROPERTYNAME(FinalTotal));
				}
			}
		}

		property String^ Notes
		{
			String^ get()
			{
				return notes;
			}
			void set(String^ val)
			{
				if (notes != val)
				{
					notes = val;
					RaisePropertyChanged(PROPERTYNAME(Notes));
				}
			}
		}

		property Date^ CustomerExpectedDate
		{
			Date^ get()
			{
				return customerExpectedDate;
			}
			void set(Date^ val)
			{
				if (customerExpectedDate != val)
				{
					customerExpectedDate = val;
					RaisePropertyChanged(PROPERTYNAME(CustomerExpectedDate));
				}
			}
		}

		static String^ GenerateNewOrderNumber();


	private:

		DECLARERAISEPROPCHANGED;
	};

	public ref class SaleOrderDetails sealed : IDTOBase
	{
		String^ id;
		String^ soid;
		String^ pid;
		uint64 pqty;
		double unitprice;
		double gross;
		double pdisc;
		DiscountTypes discType = DiscountTypes::Percent;
		double net;
		bool isexpedited;
		Date^ customerexpecteddate;
		String^ owner;

	public:

		SaleOrderDetails();
		SaleOrderDetails(String^ id);

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty {bool get(); }
		virtual property bool IsValid {bool get(); }
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ OrderId
		{
			String^ get() { return soid; }
			void set(String^ val)
			{
				if (soid != val)
				{
					soid = val;
					RaisePropertyChanged(PROPERTYNAME(OrderId));
				}
			}
		}

		property String^ ProductId
		{
			String^ get() { return pid; }
			void set(String^ val)
			{
				if (pid != val)
				{
					pid = val;
					RaisePropertyChanged(PROPERTYNAME(ProductId));
				}
			}
		}

		property uint64 Quantity
		{
			uint64 get() { return pqty; }
			void set(uint64 val)
			{
				if (pqty != val)
				{
					pqty = val;
					RaisePropertyChanged(PROPERTYNAME(Quantity));
				}
			}
		}

		property double UnitPrice
		{
			double get() { return unitprice; }
			void set(double val)
			{
				if (unitprice != val)
				{
					unitprice = val;
					RaisePropertyChanged(PROPERTYNAME(UnitPrice));
				}
			}
		}

		property double Gross
		{
			double get() { return gross; }
			void set(double val)
			{
				if (gross != val)
				{
					gross = val;
					RaisePropertyChanged(PROPERTYNAME(Gross));
				}
			}
		}

		property double Discount
		{
			double get();
			void set(double);
		}

		property DiscountTypes DiscountType
		{
			DiscountTypes get() 
			{ 
				return discType; 
			}
			void set(DiscountTypes val)
			{
				if (discType != val)
				{
					discType = val;
					RaisePropertyChanged(PROPERTYNAME(DiscountType));
				}
			}
		}

		property double Net
		{
			double get() { return net; }
			void set(double val)
			{
				if (net != val)
				{
					net = val;
					RaisePropertyChanged(PROPERTYNAME(Net));
				}
			}
		}

		property bool IsExpedited
		{
			bool get() 
			{ 
				return isexpedited ; 
			}
			void set(bool val)
			{
				if (isexpedited != val)
				{
					isexpedited = val;
					RaisePropertyChanged(PROPERTYNAME(IsExpedited));
				}
			}
		}

		property Date^ CustomerExpectedDate
		{
			Date^ get() { return customerexpecteddate; }
			void set(Date^ val)
			{
				if (customerexpecteddate != val)
				{
					customerexpecteddate = val;
					RaisePropertyChanged(PROPERTYNAME(CustomerExpectedDate));
				}
			}
		}

		property String^ Owner
		{
			String^ get() { return owner; }
			void set(String^ val)
			{
				if (owner != val)
				{
					owner = val;
					RaisePropertyChanged(PROPERTYNAME(Owner));
				}
			}
		}


	private:

		DECLARERAISEPROPCHANGED;
	};


	public ref class SalesOrderStatusHistory sealed : IDTOBase
	{
		String^ id;
		String^ soid;
		GeneralStatus^ orderstatus;
		Date^ ostatusdate;
		String^ notes;
		String^ createdby;

	public:

		SalesOrderStatusHistory();
		SalesOrderStatusHistory(String^ id);

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ OrderId
		{
			String^ get() { return soid; }
			void set(String^ val)
			{
				if (soid != val)
				{
					soid = val;
					RaisePropertyChanged(PROPERTYNAME(OrderId));
				}
			}
		}

		property GeneralStatus^ OrderStatus
		{
			GeneralStatus^ get() { return orderstatus; }
			void set(GeneralStatus^ val)
			{
				if (orderstatus != val)
				{
					orderstatus = val;
					RaisePropertyChanged(PROPERTYNAME(OrderStatus));
				}
			}
		}

		property Date^ OrderStatusDate
		{
			Date^ get() { return ostatusdate; }
			void set(Date^ val)
			{
				if (ostatusdate != val)
				{
					ostatusdate = val;
					RaisePropertyChanged(PROPERTYNAME(OrderStatusDate));
				}
			}
		}

		property String^ Notes
		{
			String^ get() { return notes; }
			void set(String^ val)
			{
				if (notes != val)
				{
					notes = val;
					RaisePropertyChanged(PROPERTYNAME(Notes));
				}
			}
		}

		property String^ CreatedBy
		{
			String^ get() { return createdby; }
			void set(String^ val)
			{
				if (createdby != val)
				{
					createdby = val;
					RaisePropertyChanged(PROPERTYNAME(CreatedBy));
				}
			}
		}


		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty {bool get(); }
		virtual property bool IsValid {bool get(); }
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;

	};

	public ref class SalesOrderItemStatusHistory sealed : IDTOBase
	{
		String^ id;
		String^ ositemid;
		String^ oslid;
		GeneralStatus^ paymentstatus;
		String^ notes;
		String^ createdby;

	public:

		SalesOrderItemStatusHistory();
		SalesOrderItemStatusHistory(String^ id);

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ OrderStatusHistoryId
		{
			String^ get() { return ositemid; }
			void set(String^ val)
			{
				if (ositemid != val)
				{
					ositemid = val;
					RaisePropertyChanged(PROPERTYNAME(OrderStatusHistoryId));
				}
			}
		}

		property String^ OrderItemId
		{
			String^ get() 
			{ 
				return oslid; 
			}
			void set(String^ val)
			{
				if (oslid != val)
				{
					oslid = val;
					RaisePropertyChanged(PROPERTYNAME(OrderItemId));
				}
			}
		}

		property GeneralStatus^ PaymentStatus
		{
			GeneralStatus^ get() { return paymentstatus; }
			void set(GeneralStatus^ val)
			{
				if (paymentstatus != val)
				{
					paymentstatus = val;
					RaisePropertyChanged(PROPERTYNAME(PaymentStatus));
				}
			}
		}

		property String^ Notes
		{
			String^ get() { return notes; }
			void set(String^ val)
			{
				if (notes != val)
				{
					notes = val;
					RaisePropertyChanged(PROPERTYNAME(Notes));
				}
			}
		}

		property String^ CreatedBy
		{
			String^ get() { return createdby; }
			void set(String^ val)
			{
				if (createdby != val)
				{
					createdby = val;
					RaisePropertyChanged(PROPERTYNAME(CreatedBy));
				}
			}
		}

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty {bool get(); }
		virtual property bool IsValid {bool get(); }
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;

	};

	public ref class SalesOrderPaymentHistory sealed : IDTOBase
	{
		String^ id;
		String^ soid;
		GeneralStatus^ paymentstatus;
		GeneralStatus^ paymentinstrument;
		Date^ paymentdate;
		String^ currency;
		double paidamount;
		String^ paydetails;

	public:

		SalesOrderPaymentHistory();
		SalesOrderPaymentHistory(String^ id);

		property String^ Id
		{
			String^ get() { return id; }
		}

		property String^ OrderId
		{
			String^ get() { return soid; }
			void set(String^ val)
			{
				if (soid != val)
				{
					soid = val;
					RaisePropertyChanged(PROPERTYNAME(OrderId));
				}
			}
		}

		property GeneralStatus^ PaymentStatus
		{
			GeneralStatus^ get() { return paymentstatus; }
			void set(GeneralStatus^ val)
			{
				if (paymentstatus != val)
				{
					paymentstatus = val;
					RaisePropertyChanged(PROPERTYNAME(PaymentStatus));
				}
			}
		}

		property GeneralStatus^ PaymentInstrument
		{
			GeneralStatus^ get() { return paymentinstrument; }
			void set(GeneralStatus^ val)
			{
				if (paymentinstrument != val)
				{
					paymentinstrument = val;
					RaisePropertyChanged(PROPERTYNAME(PaymentInstrument));
				}
			}
		}


		property String^ PayDetails
		{
			String^ get() { return paydetails; }
			void set(String^ val)
			{
				if (paydetails != val)
				{
					paydetails = val;
					RaisePropertyChanged(PROPERTYNAME(PayDetails));
				}
			}
		}

		property Date^ PaymentDate
		{
			Date^ get()
			{
				return paymentdate;
			}
			void set(Date^ val)
			{
				if (paymentdate != val)
				{
					paymentdate = val;
					RaisePropertyChanged(PROPERTYNAME(PaymentDate));
				}
			}
		}


		property String^ CurrencyId
		{
			String^ get() { return currency; }
			void set(String^ val)
			{
				if (currency != val)
				{
					currency = val;
					RaisePropertyChanged(PROPERTYNAME(CurrencyId));
				}
			}
		}

		property double PayAmount
		{
			double get() { return paidamount; }
			void set(double val)
			{
				if (paidamount != val)
				{
					paidamount = val;
					RaisePropertyChanged(PROPERTYNAME(PaidAmount));
				}
			}
		}

		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;
		virtual property DataState ObjectState;
		virtual property bool IsEmpty {bool get(); }
		virtual property bool IsValid {bool get(); }
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;

	};

}