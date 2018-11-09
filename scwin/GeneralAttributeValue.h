#pragma once

#include "typedefinitions.h"

namespace scwin
{

	public ref class GeneralAttributeValue sealed : public IProductAttributeValue
	{
	private:
		String^ id;
		Type^ type;
		Object^ value;
		IVector<Object^>^ presetValues;
		String^ uom;

	private:
		void RaisePropertyChanged(String^ propName);

	public:
		GeneralAttributeValue();

		GeneralAttributeValue(String^ id);

		virtual ~GeneralAttributeValue();

		property String ^ Id
		{
			virtual String^ get() { return id; }
		}

		property Platform::Type ^ DataType
		{
			virtual Type^ get() { return type; }
			void set(Type^ val)
			{
				if (type != val)
				{
					type = val;
					RaisePropertyChanged("DataType");
				}
			}
		}

		virtual property Object ^ Value
		{
			Object^ get() { return value; }
			void set(Object^ val)
			{
				if (value != val)
				{
					value = val;
					RaisePropertyChanged("Value");
				}
			}
		}

		property String^ Uom
		{
			String^ get()
			{
				return uom;
			}
			void set(String^ val)
			{
				if (uom != val)
				{
					uom = val;
					RaisePropertyChanged("Uom");
				}
			}
		}


		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
		
		virtual property bool CanRaiseEvents;
		
		// Inherited via IDTOBase
		virtual property DataState ObjectState;
		
		// Inherited via IProductAttributeValue
		virtual property bool IsEmpty {bool get() { return false; }}

		virtual property bool IsValid 
		{
			bool get();
		}
		


	};


}
