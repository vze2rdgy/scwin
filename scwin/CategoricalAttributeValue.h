#pragma once

#include "IProductAttributeValue.h"

using namespace Windows::Foundation::Collections;

namespace scwin
{

	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CategoricalAttributeValue sealed : public IProductAttributeValue
	{
		String^ id;
		IObservableVector<String^>^ values; // all categorical values.
		String^ value;	// the value for the selected product. Value not set when used for general purpose (not specific to a product).

	public:
		CategoricalAttributeValue();

		CategoricalAttributeValue(String^ id);

		// Inherited via IDTOBase
		virtual property DataState ObjectState;

		// Inherited via IProductAttributeValue
		virtual property bool IsEmpty
		{
			bool get() { return false; }
		}
		
		virtual property bool IsValid
		{
			bool get()
			{
				return true;
			}
		}

		virtual property String ^ Id
		{
			String^ get() { return id; }
		}
		
		virtual property Object^ Value
		{
			Object^ get();
			void set(Object^ val);
		}

		property IObservableVector<String^>^ Values
		{
			IObservableVector<String^>^ get();
		}

		property bool RaiseEvents;

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String ^ propName);

		// Inherited via IProductAttributeValue
		virtual property Platform::Type ^ DataType
		{
			Type^ get() { return String::typeid; }
		}

	};

}