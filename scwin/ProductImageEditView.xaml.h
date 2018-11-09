//
// ProductImageEditView.xaml.h
// Declaration of the ProductImageEditView class
//

#pragma once

#include "ProductViewModel.h"
#include "ProductImage.h"
#include "SharedViewModels.h"
#include "ProductImageEditView.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductImageEditView sealed 
	{
	public:
		ProductImageEditView();

		property ProductImage^ CurrentImage;

		property ProductViewModel^ ViewModel;
	};
}
