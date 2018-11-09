//
// ProductManageImages.xaml.cpp
// Implementation of the ProductManageImages class
//

#include "pch.h"
#include "ProductManageImages.xaml.h"

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

ProductManageImages::ProductManageImages()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ProductManageImages::OnLoaded);
}

void scwin::ProductManageImages::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	if (ViewModel->CurrentProduct != nullptr)
		ViewModel->LoadProductDataForPanel(1);
	ViewModel->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::ProductManageImages::OnPropertyChanged);
}

void scwin::ProductManageImages::OnApplyTemplate()
{
	UserControl::OnApplyTemplate();
}

void scwin::ProductManageImages::btnImagesEdit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	bool ret = VisualStateManager::GoToState(this, "EditState", true);
	OutputDebugString(ret.ToString()->Data());
	ViewModel->EditPanel = 1;
}


void scwin::ProductManageImages::btnImagesCancel_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ViewModel->CancelProductDataForPanel(1);
	bool ret = VisualStateManager::GoToState(this, "ViewOnlyState", true);
	OutputDebugString(ret.ToString()->Data());
	ViewModel->EditPanel = -1;
}


void scwin::ProductManageImages::btnImagesSave_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	WeakReference wr(this);
	create_task(ViewModel->SaveProductDataForPanel(1))
		.then([wr](int code)
	{
		if (code == (int)MessageIds::Success)
		{
			auto ths = wr.Resolve<ProductManageImages>();
			bool ret = VisualStateManager::GoToState(ths, "ViewOnlyState", true);
			OutputDebugString(ret.ToString()->Data());
			ths->ViewModel->EditPanel = -1;
		}
	});
}

void scwin::ProductManageImages::OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e)
{
	/*
	if (e->PropertyName == "Images")
	{

	}
	*/
}


void scwin::ProductManageImages::chkSelectImage_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto chkBox = safe_cast<CheckBox^>(sender);
	ViewModel->AreOneOreMoreImagesSelected = chkBox->IsChecked->Value;
}
