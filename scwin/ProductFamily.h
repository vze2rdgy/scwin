#pragma once
#include "dto.h"
#include "ProductPricePlan.h"
#include "ProductAttribute.h"
#include "StringHelpers.h"

namespace scwin
{

	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductFamily sealed :
		public IDTOBase
	{
		String^ id;
		String^ name;
		String^ desc;
		String^ shortDesc;
		String^ notes;
		String^ pdefattrid;
		DataState objectState;

	public:
		ProductFamily();
		ProductFamily(String^ id);

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

		property String^ DefaultAttributeId
		{
			String^ get() 
			{
				return pdefattrid;
			}
			void set(String^ val)
			{
				if (val != pdefattrid)
				{
					pdefattrid = val;
					RaisePropertyChanged("DefaultAttributeId");
				}
			}
		}

		virtual property bool CanRaiseEvents;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String^ propName);

		// Inherited via IDTOBase
		virtual property DataState ObjectState
		{
			DataState get() {
				return objectState;
			}
			void set(DataState val)
			{
				if (objectState != val)
				{
					objectState = val;
					RaisePropertyChanged("ObjectState");
				}
			}
		}

		virtual property bool IsEmpty
		{
			bool get();
		}

		virtual property bool IsValid
		{
			bool get()
			{
				if (utils::StringHelpers::IsNullOrEmpty(name))
					return false;
				return true;
			}
		}

		ProductFamily^ Clone();

	internal:
		// hack to custom notify datastate change
		void NotifyObjectStateChange();

	};

	typedef IObservableVector<ProductFamily^> ProductFamilyList;

}