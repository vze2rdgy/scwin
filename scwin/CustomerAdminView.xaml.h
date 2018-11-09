//
// CustomerAdminView.xaml.h
// Declaration of the CustomerAdminView class
//

#pragma once

#include "CustomerAdminView.g.h"
#include "CustomerViewModel.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CustomerAdminView sealed : public INotifyPropertyChanged
	{
		bool _bIsEditing = false;
		int _activePanel = -1;
		String^ _paneTitle;

	public:
		CustomerAdminView();

		property CustomerViewModel^ ViewModel;
		property CustomerDtosGroupedBindable^ Customer;

		property String^ PaneTitle
		{
			String^ get() { return _paneTitle; }
			void set(String^ val) { if (_paneTitle != val) _paneTitle = val; RaisePropertyChanged(PROPERTYNAME(PaneTitle)); }
		}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	protected:
		void OnNavigatedTo(NavigationEventArgs^ e) override;
		void OnNavigatingFrom(NavigatingCancelEventArgs^ e) override;

	private:
		void btnEditCustomer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnAddLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnAddContact_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnEditLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnEditContact_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPanelEdit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPanelCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void svCustomerDetails_PaneClosing(Windows::UI::Xaml::Controls::SplitView^ sender, Windows::UI::Xaml::Controls::SplitViewPaneClosingEventArgs^ args);
		void svCustomerDetails_PaneOpening(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args);

		DECLARERAISEPROPCHANGED;

	};
}
