//
// ProductView.xaml.h
// Declaration of the ProductView class
//

#pragma once

#include "LayoutAwarePage.h"
#include "ProductEditDetails.xaml.h"
#include "ProductView.g.h"

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductView sealed
	{
	public:
		ProductView();

		property ProductViewModel^ ViewModel;

	protected:
		void OnNavigatedTo(NavigationEventArgs^ e) override;
	};
}
