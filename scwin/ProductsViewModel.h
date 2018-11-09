#pragma once

#include "scLocalDataService.h"

namespace scwin
{

	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ProductsViewModel sealed : DependencyObject
	{
		ProductCategory^ rootCategory;

	private:

	public:
		ProductsViewModel();

	};

}
