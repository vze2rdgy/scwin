//
// NavMenuListView.h
// Declaration of the NavMenuListView class.
//
#pragma once

using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

namespace scwin
{
	public ref class NavMenuListView sealed : public Windows::UI::Xaml::Controls::ListView
	{
	private:
		SplitView^ splitViewHost;

	public:
		NavMenuListView();
		event EventHandler<ListViewItem^>^ ItemInvoked;
		void SetSelectedItem(ListViewItem^ item);

	protected:
		virtual void OnApplyTemplate() override;
		virtual void OnKeyDown(KeyRoutedEventArgs^ e) override;

	private:
		void SelectionChangedHandler(Object^ sender, SelectionChangedEventArgs^ e);
		void ItemClickHandler(Object^ sender, ItemClickEventArgs^ e);
		void InvokeItem(Object^ focusedItem);
		void OnLoaded(Object^ sender, RoutedEventArgs^ e);
		void OnPaneToggled();
		void IsOpenPanePropertyChangedCallback(DependencyObject^ sender, DependencyProperty^ args);
		void DisplayModePropertyChangedCallback(DependencyObject^ sender, DependencyProperty^ args);
		void TryMoveFocus(FocusNavigationDirection direction);

	};
}
