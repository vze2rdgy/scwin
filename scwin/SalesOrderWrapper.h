#pragma once

#include "SalesOrderDtos.h"
#include "COrgCostCenter.h"
#include "ProductWithInventoryWrapper.h"


namespace scwin
{
	public ref class SalesOrderDetailsWrapper sealed : INotifyPropertyChanged
	{
		SaleOrderDetails^ orderdet;
		String^ prodName;
		ProductImage^ titleImage;
		String^ message;
		ProductWithInventoryWrapper^ prodWrapper;

	internal:
		EventRegistrationToken PropertyChangedToken;

	public:

		SalesOrderDetailsWrapper(String^ salesOrderId);

		[Windows::Foundation::Metadata::DefaultOverload]
		SalesOrderDetailsWrapper(SaleOrderDetails^ orderdet);


		property SaleOrderDetails^ InnerItem
		{
			SaleOrderDetails^ get() { return orderdet; }
		}

		property String^ ProductName
		{
			String^ get() { return prodName; }
			void set(String^ val)
			{
				if (prodName != val)
				{
					prodName = val;
					RaisePropertyChanged(PROPERTYNAME(ProductName));
				}
			}
		}

		property ProductImage^ TitleImage
		{
			ProductImage^ get() { return titleImage; }
			void set(ProductImage^ img)
			{
				if (titleImage != img)
				{
					titleImage = img;
					RaisePropertyChanged(PROPERTYNAME(TitleImage));
				}
			}
		}

		property String^ Message
		{
			String^ get() { return message; }
			void set(String^ val)
			{
				if (message != val)
				{
					message = val;
					RaisePropertyChanged(PROPERTYNAME(Message));
				}
			}
		}

		void ComputeOrderLineItemTotal();

		property ProductWithInventoryWrapper^ ProductItem 
		{
			ProductWithInventoryWrapper^ get();
			void set(ProductWithInventoryWrapper^ val);
		}

		property bool IsTieredPricePlan;

		property double UnitPrice
		{
			double get();
			void set(double);
		}

		// quantity wrapper
		property uint64 Quantity
		{
			uint64 get();
			void set(uint64 val);
		}

		property DiscountTypes DiscountType
		{
			DiscountTypes get();
			void set(DiscountTypes);
		}

		property double Discount
		{
			double get();
			void set(double);
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:

		void LoadOrderDetailsItems();

		DECLARERAISEPROPCHANGED;

	};

	public ref class SalesOrderWrapper sealed : INotifyPropertyChanged
	{
	private:
		CustomerOrg^ customer;
		COrgCostCenter^ costcenter;
		CustomerContact^ contact;
		Address^ billingAddress;
		Address^ shippingAddress;
		SalesOrder^ order;
		Vector<SalesOrderDetailsWrapper^>^ orderDetails;
		bool isCustSelValid = false;
		GeneralStatus^ orderStatus;
		GeneralStatus^ paymentStatus;
		IVector<SalesOrderStatusHistory^>^ oshistory;
		IVector<SalesOrderPaymentHistory^>^ opHistory;
		EventRegistrationToken orderDetailsVectorChangedToken;
		double orderPreTaxTotal;
		bool hasValidChanges;
		bool hasChanges;

	public:
		SalesOrderWrapper(SalesOrder^ order);

		property GeneralStatus^ OrderStatus
		{
			GeneralStatus^ get() 
			{ 
				return orderStatus; 
			}
			void set(GeneralStatus^ val)
			{
				if (orderStatus != val)
				{
					orderStatus = val;
					RaisePropertyChanged(PROPERTYNAME(OrderStatus));
					RaisePropertyChanged(PROPERTYNAME(HasChanges));
				}
			}
		}

		property GeneralStatus^ PaymentStatus
		{
			GeneralStatus^ get() { return paymentStatus; }
			void set(GeneralStatus^ val)
			{
				if (paymentStatus != val)
				{
					paymentStatus = val;
					RaisePropertyChanged(PROPERTYNAME(PaymentStatus));
					RaisePropertyChanged(PROPERTYNAME(HasChanges));
				}
			}
		}

		property IVector<SalesOrderStatusHistory^>^ OrderStatusHistory
		{
			IVector<SalesOrderStatusHistory^>^ get();
		}

		property IVector<SalesOrderPaymentHistory^>^ PaymentStatusHistory
		{
			IVector<SalesOrderPaymentHistory^>^ get();
		}

		property CustomerOrg^ Customer
		{
			CustomerOrg^ get() { return customer; }
			void set(CustomerOrg^ val)
			{
				if (customer != val)
				{
					customer = val;
					RaisePropertyChanged(PROPERTYNAME(Customer));
					IsCustomerSelectionValid = costcenter != nullptr && customer != nullptr;
					RaisePropertyChanged(PROPERTYNAME(HasChanges));
				}
			}
		}

		property COrgCostCenter^ CostCenter
		{
			COrgCostCenter^ get() { return costcenter; }
			void set(COrgCostCenter^ val)
			{
				if (costcenter != val)
				{
					costcenter = val;
					if (costcenter != nullptr)
						order->CostCenterId = costcenter->Id;
					else
						order->CostCenterId = nullptr;
					RaisePropertyChanged(PROPERTYNAME(CostCenter));
					IsCustomerSelectionValid = costcenter != nullptr && customer != nullptr;
					UpdateCostCenterAddress();
					RaisePropertyChanged(PROPERTYNAME(HasChanges));
				}
			}
		}

		property CustomerContact^ Contact
		{
			CustomerContact^ get() { return contact; }
			void set(CustomerContact^ val)
			{
				if (contact != val)
				{
					contact = val;
					if (contact != nullptr)
						order->ContactId = contact->Id;
					else
						order->ContactId = nullptr;
					RaisePropertyChanged(PROPERTYNAME(Contact));
					IsCustomerSelectionValid = costcenter != nullptr && customer != nullptr && contact != nullptr;
					RaisePropertyChanged(PROPERTYNAME(HasChanges));
				}
			}
		}

		property Address^ BillingAddress
		{
			Address^ get() 
			{ 
				return billingAddress; 
			}
			void set(Address^ val)
			{
				if (billingAddress != val)
				{
					billingAddress = val;
					RaisePropertyChanged(PROPERTYNAME(BillingAddress));
				}
			}
		}
		property Address^ ShippingAddress
		{
			Address^ get()
			{
				return shippingAddress;
			}
			void set(Address^ val)
			{
				if (shippingAddress != val)
				{
					shippingAddress = val;
					RaisePropertyChanged(PROPERTYNAME(ShippingAddress));
				}
			}
		}

		property bool IsCustomerSelectionValid
		{
			bool get() { return isCustSelValid; }
		private:
			void set(bool val)
			{
				if (isCustSelValid != val)
				{
					isCustSelValid = val;
					RaisePropertyChanged(PROPERTYNAME(IsCustomerSelectionValid));
				}
			}
		}

		property bool IsExistingOrder
		{
			bool get();
		}

		// are there changes in the inner order object and 
		// is the order valid ?
		property bool HasValidChanges
		{
			bool get();
		}

		// Is there any change with the inner order object ?
		property bool HasChanges
		{
			bool get();
		}

		property bool CanRevert
		{
			bool get();
		}

		property SalesOrder^ Order
		{
			SalesOrder^ get();
		private:
			void set(SalesOrder^);
		}

		property IVector<SalesOrderDetailsWrapper^>^ OrderDetails
		{
			IVector<SalesOrderDetailsWrapper^>^ get();
			void set(IVector<SalesOrderDetailsWrapper^>^);
		}

		property DiscountTypes OrderDiscountType
		{
			DiscountTypes get();
			void set(DiscountTypes);
		}

		property double OrderTotalBeforeTaxesAndDiscounts
		{
			double get();
			void set(double);
		}

		property double OrderDiscount
		{
			double get();
			void set(double);
		}

		property double OrderPreTaxTotal
		{
			double get();
			void set(double);
		}

		property double OrderFinal
		{
			double get();
			void set(double);
		}

		property double OrderTaxPct
		{
			double get();
			void set(double);
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void ResetChangeStatus();

	private:
		DECLARERAISEPROPCHANGED;

		void UpdateOrderValidity();

		void UpdateCostCenterAddress();

		void OrderDetailsVectorChanged(IObservableVector<SalesOrderDetailsWrapper^>^ v, IVectorChangedEventArgs^ args);
		
		void OrderDetailItemPropertyChanged(Object^ sender, PropertyChangedEventArgs^ eargs);

		void CalculateOrderTotals();

		void OnOrderPropertyChanged(Object^ sender, PropertyChangedEventArgs^ eargs);

		void FetchWrapperNeededData();

	};



}