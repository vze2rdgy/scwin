#pragma once

#include "IProductAttributeValue.h"

namespace scwin
{

	ref class Product;

	Type^ LookupAttributeValueType(String^ attributeType);

	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductAttribute sealed : IDTOBase
	{
		String^ id;
		String^ paid;
		String^ name;
		String^ desc;
		String^ type;	// See ProductAttributeTypes type for individual values.
		bool bAffectsPrice; // when true, this attribute may result in different pricing.

	public:
		ProductAttribute();

		ProductAttribute(String^ id);

		property String^ Id
		{
			String^ get() 
			{ 
				return this->id; 
			}
		}

		property String^ ParentId
		{
			String^ get()
			{
				return paid;
			}
			void set(String^ val)
			{
				if (val != paid)
				{
					paid = val;
					RaisePropertyChanged("ParentId");
				}
			}
		}

		property String^ Name
		{
			String^ get() { return this->name; }
			void set(String^ val)
			{
				if (name != val)
				{
					name = val;
					RaisePropertyChanged("Name");
				}
			}
		}

		property String^ Description
		{
			String^ get() { return this->desc; }
			void set(String^ val)
			{
				if (desc != val)
				{
					desc = val;
					RaisePropertyChanged("Description");
				}
			}
		}

		property String^ Type
		{
			String^ get() { return this->type; }
			void set(String^ val)
			{
				if (type != val)
				{
					type = val;
					RaisePropertyChanged("Type");
				}
			}
		}

		//property bool AffectsPrice
		//{
		//	bool get() { return this->bAffectsPrice; }
		//	void set(bool val)
		//	{
		//		if (bAffectsPrice != val)
		//		{
		//			bAffectsPrice = val;
		//			RaisePropertyChanged("AffectsPrice");
		//		}
		//	}
		//}

		virtual property bool CanRaiseEvents;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String^ propName);

		// Inherited via IDTOBase
		virtual property DataState ObjectState;

		virtual property bool IsEmpty
		{
			bool get()
			{
				return false;
			}
		}

		virtual property bool IsValid
		{
			bool get();
		}

};

	typedef IObservableVector<ProductAttribute^> ProductAttributeList;

}