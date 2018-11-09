#pragma once
#include "ProductFamily.h"

using namespace scwin::utils;

namespace scwin
{


	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductCategory sealed : IDTOBase
	{
		String^ id;
		String^ name;
		String^ desc;
		String^ parentId;

		IObservableVector<ProductCategory^>^ children;
		ProductCategory^ parent;
		Object^ tag;

	public:

		ProductCategory();
		ProductCategory(String^ id);

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void RaisePropertyChanged(String^ propName);

		property String^ Id
		{
			String^ get() 
			{ 
				return id;
			}
			void set(String^ val)
			{
				if (id != val)
				{
					id = val;
					this->RaisePropertyChanged("Id");
				}
			}
		}

		property String^ Name
		{
			String^ get()
			{
				return name;
			}
			void set(String^ val)
			{
				if (name != val)
				{
					name = val;
					this->RaisePropertyChanged("Name");
				}
			}
		}

		property String^ Desc
		{
			String^ get()
			{
				return desc;
			}
			void set(String^ val)
			{
				if (desc != val)
				{
					desc = val;
					this->RaisePropertyChanged("Desc");
				}
			}
		}

		property ProductCategory^ Parent
		{
			ProductCategory^ get() { return parent; }
			void set(ProductCategory^ val)
			{
				if (val != parent)
				{
					parent = val;
					RaisePropertyChanged("Parent");
				}
			}
		}

		property String^ ParentId
		{
			String^ get() 
			{ 
				return parentId; 
			}
			void set(String^ pid)
			{
				if (parentId != pid)
				{
					parentId = pid;
				}
			}
		}

		/// <summary> When set to true, the method children list returned by 
		/// GetCategoryChildren() will be held in this class.
		/// </summary>
		property bool KeepChildren;

		property bool HasChildren;

		property Object^ Tag
		{
			Object^ get() { return tag; }
			void set(Object^ val)
			{
				if (tag != val)
				{
					tag = val;
					RaisePropertyChanged("Tag");
				}
			}
		}

		property int ChildrenCount
		{
			int get();
		}

		IAsyncOperationWithProgress<IObservableVector<ProductCategory^>^, int>^ GetCategoryChildren();

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



		// Inherited via IDTOBase
		virtual property bool CanRaiseEvents;

};
}