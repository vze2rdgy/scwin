//
// ProductManageImages.xaml.h
// Declaration of the ProductManageImages class
//

#pragma once

#include "ProductImage.h"
#include "ProductManageImages.g.h"
#include "SharedViewModels.h"
#include "ProductViewModel.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductManageImages sealed
	{
	public:
		ProductManageImages();

		property ProductViewModel^ ViewModel;

	protected:
		virtual void OnApplyTemplate() override;

	private:
		void btnImagesEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnImagesCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnImagesSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e);
		void chkSelectImage_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
