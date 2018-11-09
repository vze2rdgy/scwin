#pragma once

#include "typedefinitions.h"
#include "ProductAttributeAndValue.h"


namespace scwin
{

	[Bindable]
	public ref class AttributeViewModelFacade sealed : public DependencyObject, public INotifyPropertyChanged
	{
	private:
		Product^ product; // currently selected product.
		IVector<ProductAttributeAndValue^>^ prodAttributes;
		VisualStateTriggers vstrigger = VisualStateTriggers::NoDataState;
		ProductFamily^ family;

	public:
		AttributeViewModelFacade(Product^ product);
		virtual ~AttributeViewModelFacade();

		void Initialize();

		void UpdateState();

		void Reset();

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property VisualStateTriggers VisualStateTrigger
		{
			VisualStateTriggers get()
			{
				return vstrigger;
			}
			void set(VisualStateTriggers val)
			{
				if (vstrigger != val)
				{
					vstrigger = val;
					InvokePropertyChanged("VisualStateTrigger");
				}
			}
		}

		property Product^ SelectedProduct
		{
			Product^ get() 
			{ 
				return product; 
			}
			void set(Product^ val) 
			{
				if (product != val)
				{
					product = val;
					InvokePropertyChangedAsync("Product");
				}
			}
		}

		property String^ FamilyName;

		/// <summary> Update the product family without raising notification. </summary>
		void UpdateFamily(ProductFamily^ fam);

		void UpdateFamilyName(String^ val);

		property ProductFamily^ Family
		{
			ProductFamily^ get()
			{ 
				return this->family;
			}
			void set(ProductFamily^ val)
			{
				if (family != val)
				{
					family = val;
					if (family != nullptr)
					{
						FamilyName = family->Name;
					}
					else
					{
						FamilyName = nullptr;
					}
					InvokePropertyChangedAsync("Family");
					InvokePropertyChangedAsync("FamilyName");
				}
			}
		}

		property IVector<ProductAttributeAndValue^>^ ProductAttributes
		{
			IVector<ProductAttributeAndValue^>^ get()
			{
				return prodAttributes;
			}

			void set(IVector<ProductAttributeAndValue^>^ val)
			{
				if (prodAttributes != val)
				{
					prodAttributes = val;
					InvokePropertyChangedAsync("ProductAttributes");
				}
			}
		}

		IAsyncOperation<IVector<ProductFamily^>^>^ GetMatchingFamiliesAsync(String^ familyNameOrDesc);

	private:

		IAsyncAction^ LoadAttribsAsync();

		void InvokePropertyChangedAsync(String^ propName);
		void InvokePropertyChanged(String^ propName);

	};


}
