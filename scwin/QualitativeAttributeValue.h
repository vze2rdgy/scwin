#pragma once
#include "IProductAttributeValue.h"

namespace scwin
{

	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class QualitativeAttributeValue sealed :
		public IProductAttributeValue
	{
		String^ id;
		String^ value;	// the value for the selected product. Value not set when used for general purpose (not specific to a product).

	public:
		QualitativeAttributeValue();

		QualitativeAttributeValue(String^ id);

		virtual property String ^ Id
		{
			String^ get() { return id; }
		}

		virtual property Object^ Value
		{
			Object^ get();
			void set(Object^ val);
		}

		virtual property Platform::Type ^ DataType { Type^ get() { return Object::typeid; }}

		property bool RaiseEvents;

		void RaisePropertyChanged(String ^ propName);

		// Inherited via IDTOBase
		virtual property DataState ObjectState;

		// Inherited via IProductAttributeValue
		virtual property bool IsEmpty { bool get() { return false; }}

		virtual property bool IsValid
		{
			bool get()
			{
				return true;
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};


}