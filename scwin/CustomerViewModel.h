#pragma once

#include "CustomerDtosGroupedBindable.h"

namespace scwin
{

	[Bindable]
	public ref class CustomerViewModel sealed : public INotifyPropertyChanged
	{
		CustomerDtosGroupedBindable^ current;
		IVector<CustomerDtosGroupedBindable^>^ orgs;
		GridOrListViewViewType _lastViewType;
		Platform::Object^ _activeDataContext;
		Platform::String^ _activeDataTemplateName;
		bool _bIsCommited = false;

	private:
		DECLARERAISEPROPCHANGED;

		void RestoreState();

	public:
		CustomerViewModel();

		virtual ~CustomerViewModel();

		property CustomerDtosGroupedBindable^ Current
		{
			CustomerDtosGroupedBindable^ get() { return current; }
			void set(CustomerDtosGroupedBindable^ val)
			{
				if (val != current)
				{
					current = val;
					RaisePropertyChanged("Current");
				}
			}
		}


		property IVector<CustomerDtosGroupedBindable^>^ Organizations
		{
			IVector<CustomerDtosGroupedBindable^>^ get() 
			{
				return orgs; 
			}
			void set(IVector<CustomerDtosGroupedBindable^>^ val)
			{
				if (orgs != val)
				{
					orgs = val;
					RaisePropertyChanged("Organizations");
				}
			}
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property DataState CurrentAction;

		property GridOrListViewViewType LastViewType
		{
			GridOrListViewViewType get() { return _lastViewType; }
			void set(GridOrListViewViewType val)
			{
				if (_lastViewType != val)
				{
					_lastViewType = val;
					RaisePropertyChanged(PROPERTYNAME(LastViewType));
				}
			}
		}

		property Platform::Object^ ActiveDataContext
		{
			Platform::Object^ get();
			void set(Platform::Object^ val);
		}

		property Platform::String^ ActiveDataTemplateName
		{
			Platform::String^ get();
		private:
			void set(Platform::String^ val);
		}

		void SaveState();
	};

}
