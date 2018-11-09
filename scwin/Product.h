#pragma once

#include "ProductPricePlan.h"
#include "ProductAttribute.h"
#include "ProductImage.h"

using namespace Windows::UI::Xaml::Media;

namespace scwin
{

	ref class ProductAttributeAndValue;


	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Product sealed : IDTOBase
	{
		String^ id;
		String^ name;
		String^ skun;
		String^ subName;
		String^ desc;
		String^ shortDesc;
		String^ notes;
		String^ pricePlanId;
		Array<byte>^ barCode;
		bool isStockLess;
		String^ uom;
		ProductFamily^ pfamily;
		ProductImage^ img;
		bool isTitleImageLoading = false; // helper flag.

	public:
		Product();
		Product(String^ id);

		property String^ Id
		{
			String^ get() { return this->id; }
		}

		property String^ Name
		{
			String^ get() 
			{ 
				return this->name; 
			}
			void set(String^ val) {
				if (val != name)
				{
					name = val;
					RaisePropertyChanged("Name");
				}
			}
		}

		property String^ SKUN
		{
			String^ get() { return this->skun; }
			void set(String^ val) 
			{
				if (val != skun)
				{
					skun = val;
					RaisePropertyChanged("SKUN");
				}
			}
		}
		
		property String^ SubName
		{
			String^ get() { return this->subName; }
			void set(String^ val)
			{
				if (val != subName)
				{
					subName = val;
					RaisePropertyChanged("SubName");
				}
			}
		}

		property String^ Description
		{
			String^ get() { return this->desc; }
			void set(String^ val)
			{
				if (val != desc)
				{
					desc = val;
					RaisePropertyChanged("Description");
				}
			}
		}

		property String^ ShortDescription
		{
			String^ get() { return this->shortDesc; }
			void set(String^ val)
			{
				if (val != shortDesc)
				{
					shortDesc = val;
					RaisePropertyChanged("ShortDescription");
				}
			}
		}
		property String^ Notes
		{
			String^ get() { return this->notes; }
			void set(String^ val)
			{
				if (val != notes)
				{
					notes = val;
					RaisePropertyChanged("Notes");
				}
			}
		}

		property String^ PricePlanId
		{
			String^ get() { return this->pricePlanId; }
			void set(String^ val)
			{
				if (val != pricePlanId)
				{
					pricePlanId = val;
					RaisePropertyChanged("PricePlanId");
				}
			}
		}

		Array<byte>^ GetBarCode()
		{
			return this->barCode;
		}

		void SetBarCode(const Array<byte>^ val)
		{
			if (val != barCode || (barCode != nullptr && val->Length != barCode->Length))
			{
				barCode = const_cast<Array<byte>^>(val);
				RaisePropertyChanged("BarCode");
			}
		}

		///Declares this product as directly supplied by an external supplier or manufacturer
		property bool IsStockLess
		{
			bool get() { return this->isStockLess; }
			void set(bool val)
			{
				if (val != isStockLess)
				{
					isStockLess = val;
					RaisePropertyChanged("IsStockLess");
				}
			}
		}

		///Unit of measurement.
		property String^ UOM
		{
			String^ get() { return this->uom; }
			void set(String^ val)
			{
				if (val != uom)
				{
					uom = val;
					RaisePropertyChanged("UOM");
				}
			}
		}

		property ProductFamily^ Family
		{
			ProductFamily^ get()
			{
				return pfamily;
			}
			void set(ProductFamily^ val)
			{
				if (pfamily != val)
				{
					pfamily = val;
					RaisePropertyChanged("Family");
				}
			}
		}

		property ProductImage^ TitleImage
		{
			ProductImage^ get();
			void set(ProductImage^);
		}

		property bool IsTitleImageLoading
		{
			bool get() { return this->isTitleImageLoading; }
			void set(bool flag)
			{
				isTitleImageLoading = flag;
				RaisePropertyChanged("IsTitleImageLoading");
			}
		}

		IAsyncOperation<IVector<ProductAttributeAndValue^>^>^ GetAttributes();

		virtual property bool CanRaiseEvents;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String^ propName);

		// Inherited via IDTOBase
		virtual property DataState ObjectState;

		virtual property bool IsEmpty
		{
			bool get();
		}

		virtual property bool IsValid
		{
			bool get()
			{
				return true;
			}
		}

		// Inherited via ICloneable
		Product^ Clone();

};


	typedef IObservableVector<Product^> ProductList;

}