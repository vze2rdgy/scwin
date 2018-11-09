#pragma once

#include "GeneralAttributeValue.h"

namespace scwin
{
	[Bindable]
	public ref class ProductAttributeAndValue sealed : INotifyPropertyChanged
	{
		bool isdefAttrib;
	public:
		ProductAttributeAndValue();

		ProductAttributeAndValue(UINT ordinal, ProductAttribute^ attribute, GeneralAttributeValue^ attrvalue);

		/// <summary>The order of the attribute in the list starting from 1. This is for presentation purpose only </summary>
		property UINT Ordinal; 

		/// <summary>Attribute</summary>
		property ProductAttribute^ Attribute;

		/// <summary>Attribute Value</summary>
		property GeneralAttributeValue^ Value;

		property bool IsProductDefaultAttribute
		{
			bool get() 
			{
				return isdefAttrib;
			}
			void set(bool val)
			{
				isdefAttrib = val;
				RaisePropertyChanged("IsProductDefaultAttribute");
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property bool CanRaiseEvents;

		property DataState ObjectState;

	private:
		void RaisePropertyChanged(String^ propName);
	};

}