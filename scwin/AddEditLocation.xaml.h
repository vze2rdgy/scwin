//
// AddEditLocation.xaml.h
// Declaration of the AddEditLocation class
//

#pragma once

#include "AddEditLocation.g.h"
#include "CustomerViewControl.xaml.h"
#include "CustomerDtosGroupedBindable.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AddEditLocation sealed : public INotifyPropertyChanged
	{
		CostCenterDtosGroupedBindable^ _location;

	public:
		AddEditLocation();

		property CostCenterDtosGroupedBindable^ Location
		{
			CostCenterDtosGroupedBindable^ get();
			void set(CostCenterDtosGroupedBindable^ val);
		}

		property String^ Title;

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	private:
		DECLARERAISEPROPCHANGED;
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void cmbShippingSameAs_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void cmbBillingSameAs_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void SettleSameAsAddress(String^ addrkey, String^ selection);
	};
}
