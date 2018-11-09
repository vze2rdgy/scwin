//
// FamilySetupLookup.xaml.h
// Declaration of the FamilySetupLookup class
//

#pragma once

#include "ProductFamilies.xaml.h"
#include "FamilySetupLookup.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class FamilySetupLookup sealed : INotifyPropertyChanged
	{
		AttributeViewModelFacade^ attribfacade;

	public:
		FamilySetupLookup(AttributeViewModelFacade^ attribfacade);



		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property AttributeViewModelFacade^ AttributeFacade
		{
			AttributeViewModelFacade^ get()
			{
				return attribfacade;
			}
		}


	private:

		void RaisePropertyChanged(String^ propName);

		void btnAccept_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void btnCloseWindow_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	};
}
