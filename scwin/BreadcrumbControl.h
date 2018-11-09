#pragma once

#include "BreadcrumbEventArgs.h"
#include "BreadcrumbItem.h"

using namespace Windows::Foundation::Collections;

namespace scwin
{

	public ref class BreadcrumbControl sealed : public ContentControl
	{
		static ResourceDictionary^ _styleResources;
		static DependencyProperty^ _HomeIconProperty;
		static DependencyProperty^ _ItemsSourceProperty;
		static DependencyProperty^ _ItemsProperty;
		static DependencyProperty^ _HomeTemplateProperty;
		static DependencyProperty^ _ItemTemplateProperty;
		static DependencyProperty^ _SeperatorTemplateProperty;
		static DependencyProperty^ _OverFlowTemplateProperty;
		static DependencyProperty^ _DisplayMemberPathProperty;
		static DependencyProperty^ _SeperatorProperty;
		static DependencyProperty^ _HomeTextProperty;
		static DependencyProperty^ _OverFlowProperty;
		static DependencyProperty^ _ItemStyleProperty;
		static DependencyProperty^ _HomeItemStyleProperty;
		static DependencyProperty^ _ItemCommandProperty;
		static DependencyProperty^ _HomeCommandProperty;
		EventRegistrationToken _itemsSourceNotifyChangedEventToken;
		StackPanel^ _stackPanel;
		event RoutedEventHandler^ _homeSelected;
		event BreadcrumbEventHandler^ _itemSelected;

	public:
		BreadcrumbControl();

		static void Register();

		event RoutedEventHandler^ HomeSelected
		{
			EventRegistrationToken add(RoutedEventHandler^ evnt);
			void remove(EventRegistrationToken);
			void raise(Object^ sender, RoutedEventArgs^ args);
		}

		event BreadcrumbEventHandler^ ItemSelected
		{
			EventRegistrationToken add(BreadcrumbEventHandler^ evnt);
			void remove(EventRegistrationToken);
			void raise(BreadcrumbControl^ sender, BreadcrumbEventArgs^ args);
		}

		static property DependencyProperty^ HomeIconProperty
		{
			DependencyProperty^ get() { return _HomeIconProperty; }
		}

		property SymbolIcon^ HomeIcon
		{
			SymbolIcon^ get() { return (SymbolIcon^)GetValue(_HomeIconProperty); }
			void set(SymbolIcon^ val) { SetValue(_HomeIconProperty, val); }
		}

		static property DependencyProperty^ ItemsSourceProperty
		{
			DependencyProperty^ get() { return _ItemsSourceProperty; }
		}

		property Object^ ItemsSource
		{
			Object^ get() { return (Object^)GetValue(_ItemsSourceProperty); }
			void set(Object^ val) { SetValue(_ItemsSourceProperty, val); }
		}

		static property DependencyProperty^ ItemsProperty
		{
			DependencyProperty^ get() { return _ItemsProperty; }
		}

		property IObservableVector<Object^>^ Items
		{
			IObservableVector<Object^>^ get() { return (ItemCollection^)GetValue(_ItemsProperty); }
			void set(IObservableVector<Object^>^ val) { SetValue(_ItemsProperty, val); }
		}

		static property DependencyProperty^ HomeTemplateProperty
		{
			DependencyProperty^ get() { return _HomeTemplateProperty; }
		}

		property DataTemplate^ HomeTemplate
		{
			DataTemplate^ get() { return (DataTemplate^)GetValue(_HomeTemplateProperty); }
			void set(DataTemplate^ val) { SetValue(_HomeTemplateProperty, val); }
		}

		static property DependencyProperty^ ItemTemplateProperty
		{
			DependencyProperty^ get() { return _ItemsSourceProperty; }
		}

		property DataTemplate^ ItemTemplate
		{
			DataTemplate^ get() { return (DataTemplate^)GetValue(_ItemTemplateProperty); }
			void set(DataTemplate^ val) { SetValue(_ItemTemplateProperty, val); }
		}

		static property DependencyProperty^ SeperatorTemplateProperty
		{
			DependencyProperty^ get() { return _SeperatorTemplateProperty; }
		}

		property DataTemplate^ SeperatorTemplate
		{
			DataTemplate^ get() { return (DataTemplate^)GetValue(_SeperatorTemplateProperty); }
			void set(DataTemplate^ val) { SetValue(_SeperatorTemplateProperty, val); }
		}

		static property DependencyProperty^ OverFlowTemplateProperty
		{
			DependencyProperty^ get() { return _OverFlowTemplateProperty; }
		}

		property DataTemplate^ OverFlowTemplate
		{
			DataTemplate^ get() { return (DataTemplate^)GetValue(_OverFlowTemplateProperty); }
			void set(DataTemplate^ val) { SetValue(_OverFlowTemplateProperty, val); }
		}

		static property DependencyProperty^ DisplayMemberPathProperty
		{
			DependencyProperty^ get() { return _DisplayMemberPathProperty; }
		}

		property String^ DisplayMemberPath
		{
			String^ get() { return (String^)GetValue(_DisplayMemberPathProperty); }
			void set(String^ val) { SetValue(_DisplayMemberPathProperty, val); }
		}

		static property DependencyProperty^ SeperatorProperty
		{
			DependencyProperty^ get() { return _SeperatorProperty; }
		}

		property String^ Seperator
		{
			String^ get() { return (String^)GetValue(_SeperatorProperty); }
			void set(String^ val) { SetValue(_SeperatorProperty, val); }
		}

		static property DependencyProperty^ HomeTextProperty
		{
			DependencyProperty^ get() { return _HomeTextProperty; }
		}

		property String^ HomeText
		{
			String^ get() { return (String^)GetValue(_HomeTextProperty); }
			void set(String^ val) { SetValue(_HomeTextProperty, val); }
		}

		static property DependencyProperty^ OverFlowProperty
		{
			DependencyProperty^ get() { return _OverFlowProperty; }
		}

		property String^ OverFlow
		{
			String^ get() { return (String^)GetValue(_OverFlowProperty); }
			void set(String^ val) { SetValue(_OverFlowProperty, val); }
		}

		static property DependencyProperty^ ItemStyleProperty
		{
			DependencyProperty^ get() { return _ItemStyleProperty; }
		}

		property Windows::UI::Xaml::Style^ ItemStyle
		{
			Windows::UI::Xaml::Style^ get() { return (Windows::UI::Xaml::Style^)GetValue(_ItemStyleProperty); }
			void set(Windows::UI::Xaml::Style^ val) { SetValue(_ItemStyleProperty, val); }
		}

		static property DependencyProperty^ HomeItemStyleProperty
		{
			DependencyProperty^ get() { return _HomeItemStyleProperty; }
		}

		property Windows::UI::Xaml::Style^ HomeItemStyle
		{
			Windows::UI::Xaml::Style^ get() { return (Windows::UI::Xaml::Style^)GetValue(_HomeItemStyleProperty); }
			void set(Windows::UI::Xaml::Style^ val) { SetValue(_HomeItemStyleProperty, val); }
		}

		static property DependencyProperty^ ItemCommandProperty
		{
			DependencyProperty^ get() { return _ItemCommandProperty; }
		}

		property ICommand^ ItemCommand
		{
			ICommand^ get() { return (ICommand^)GetValue(_ItemCommandProperty); }
			void set(ICommand^ val) { SetValue(_ItemCommandProperty, val); }
		}

		static property DependencyProperty^ HomeCommandProperty
		{
			DependencyProperty^ get() { return _HomeCommandProperty; }
		}

		property ICommand^ HomeCommand
		{
			ICommand^ get() { return (ICommand^)GetValue(_HomeCommandProperty); }
			void set(ICommand^ val) { SetValue(_HomeCommandProperty, val); }
		}

	protected:
		virtual void OnApplyTemplate() override;
		virtual void OnHomeSelected();
		virtual void OnItemSelected(BreadcrumbEventArgs^ eargs);


	private:
		static void OnItemsSourcePropertyChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e);
		void OnItemsSourceChanged(IObservableVector<Object^>^ oldValue, IObservableVector<Object^>^ newValue);
		void ItemsOnCollectionChanged(IObservableVector<Object^>^ sender, IVectorChangedEventArgs^ eargs);
		void SetSeperatorItem(UIElementCollection^ collection, DataTemplate^ seperatorTemplate, String^ seperator);
		void SetOverflowItem(BreadcrumbItem^, DataTemplate^ overflowTemplate, String^ overFlow);
		void SetItem(UIElementCollection^ collection, Object^ item, IObservableVector<Object^>^ items, DataTemplate^ itemTemplate, String^ displayMemberPath);
		BreadcrumbItem^ CreateButton();
		void SetHomeItem(UIElementCollection^ collection);
		ResourceDictionary^ GetStyleResources();
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
};

	//https://github.com/goedware/UWP-Breadcrumb-Control/blob/master/GoedWare.Controls.Breadcrumb/GoedWare.Controls.Breadcrumb/BreadcrumbControl.cs

}