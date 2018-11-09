//
// GridOrListView.h
// Declaration of the GridOrListView class.
//

#pragma once

namespace scwin
{

	public ref class GridOrListView sealed : public Windows::UI::Xaml::Controls::ListView
	{
		static DependencyProperty^ _listViewItemTemplateProperty;
		static DependencyProperty^ _gridViewItemTemplateProperty;
		static DependencyProperty^ _defaultViewTypeProperty;
		static DependencyProperty^ _currentViewTypeProperty;
		static DependencyProperty^ _displayAddButtonProperty;
		static DependencyProperty^ _addButtonLabelProperty;
		static DependencyProperty^ _addButtonTooltipProperty;
		AppBarToggleButton^ _btnSwitch;
		event TypedEventHandler<ListView^, String^>^ _searchTextChanged; // using ListView to prevent Compiler Warning (level 1) C4462
		event RoutedEventHandler^ _addButtonClicked;
		bool _bSuppressNotification = false;

	public:
		static void Register(); // register dps
		
		GridOrListView();

		static property DependencyProperty^ ListViewItemTemplateProperty
		{
			DependencyProperty^ get() { return _listViewItemTemplateProperty; }
		}

		static property DependencyProperty^ GridViewItemTemplateProperty
		{
			DependencyProperty^ get() { return _gridViewItemTemplateProperty; }
		}

		static property DependencyProperty^ DefaultViewTypeProperty
		{
			DependencyProperty^ get() { return _defaultViewTypeProperty; }
		}

		static property DependencyProperty^ CurrentViewTypeProperty
		{
			DependencyProperty^ get() { return _currentViewTypeProperty; }
		}

		static property DependencyProperty^ DisplayAddButtonProperty
		{
			DependencyProperty^ get() { return _displayAddButtonProperty; }
		}

		static property DependencyProperty^ AddButtonLabelProperty
		{
			DependencyProperty^ get() { return _addButtonLabelProperty; }
		}

		static property DependencyProperty^ AddButtonTooltipProperty
		{
			DependencyProperty^ get() { return _addButtonTooltipProperty; }
		}

		property DataTemplate^ ListViewItemTemplate
		{
			DataTemplate^ get() { return (DataTemplate^)GetValue(_listViewItemTemplateProperty); }
			void set(DataTemplate^ val) { SetValue(_listViewItemTemplateProperty, val); }
		}

		property DataTemplate^ GridViewItemTemplate
		{
			DataTemplate^ get() { return (DataTemplate^)GetValue(_gridViewItemTemplateProperty); }
			void set(DataTemplate^ val) { SetValue(_gridViewItemTemplateProperty, val); }
		}

		property GridOrListViewViewType DefaultViewType
		{
			GridOrListViewViewType get() 
			{ 
				return (GridOrListViewViewType)GetValue(_defaultViewTypeProperty);
			}
			void set(GridOrListViewViewType val)
			{
				SetValue(_defaultViewTypeProperty, val);
			}
		}

		property GridOrListViewViewType CurrentViewType
		{
			GridOrListViewViewType get()
			{
				return (GridOrListViewViewType)GetValue(_currentViewTypeProperty);
			}
			void set(GridOrListViewViewType val)
			{
				SetValue(_currentViewTypeProperty, val);
			}
		}

		property Windows::UI::Xaml::Visibility DisplayAddButton
		{
			Windows::UI::Xaml::Visibility get() { return (Windows::UI::Xaml::Visibility)GetValue(_displayAddButtonProperty); }
			void set(Windows::UI::Xaml::Visibility val) { SetValue(_displayAddButtonProperty, val); }
		}

		property String^ AddButtonLabel
		{
			String^ get() { return (String^)GetValue(_addButtonLabelProperty); }
			void set(String^ val) { SetValue(_addButtonLabelProperty, val); }
		}

		property String^ AddButtonTooltip
		{
			String^ get() { return (String^)GetValue(_addButtonTooltipProperty); }
			void set(String^ val) { SetValue(_addButtonTooltipProperty, val); }
		}

		event TypedEventHandler<ListView^, String ^>^ SearchKeyChanged
		{
			EventRegistrationToken add(TypedEventHandler<ListView^, String ^>^ handler)
			{
				return _searchTextChanged += handler;
			}

			void remove(EventRegistrationToken token)
			{
				_searchTextChanged -= token;
			}

			void raise(ListView^ sender, String^ eargs)
			{
				_searchTextChanged(this, eargs);
			}
		}

		event RoutedEventHandler^ AddButtonClicked
		{
			EventRegistrationToken add(RoutedEventHandler^ handler)
			{
				return _addButtonClicked += handler;
			}

			void remove(EventRegistrationToken token)
			{
				_addButtonClicked -= token;
			}

			void raise(Object^ sender, RoutedEventArgs^ eargs)
			{
				_addButtonClicked(this, eargs);
			}
		}

	protected:
		virtual void OnApplyTemplate() override;

	private:
		static void OnCurrentViewTypeChanged(DependencyObject^ db, DependencyPropertyChangedEventArgs^ eargs);
		static void OnItemViewTemplateChange(DependencyObject^ db, DependencyPropertyChangedEventArgs^ eargs);
		void OnAsbSearchChanged(AutoSuggestBox^ sender, AutoSuggestBoxTextChangedEventArgs^ eargs);
		void OnbtnToggleToGridOrListChecked(Object^ sender, RoutedEventArgs^ eargs);
		void OnbtnToggleToGridOrListUnchecked(Object^ sender, RoutedEventArgs^ eargs);
		void OnAddButtonClicked(Object^ sender, RoutedEventArgs^ eargs);
		void UpdateView();
	};

}
