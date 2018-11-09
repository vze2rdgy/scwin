//
// ProductView.xaml.cpp
// Implementation of the ProductView class
//

#include "pch.h"
#include "ProductView.xaml.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

ProductView::ProductView()
{
	InitializeComponent();
}

void scwin::ProductView::OnNavigatedTo(NavigationEventArgs ^ e)
{
	Page::OnNavigatedTo(e);
	ViewModel = dynamic_cast<ProductViewModel^>(e->Parameter);
}

