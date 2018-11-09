//
// PictureSelection.xaml.h
// Declaration of the PictureSelection class
//

#pragma once

#include "PictureSelection.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PictureSelection sealed
	{
	public:
		PictureSelection();
	private:
		void btnDelete_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnClose_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnAccept_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};
}
