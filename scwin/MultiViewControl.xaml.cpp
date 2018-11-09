//
// MultiViewControl.xaml.cpp
// Implementation of the MultiViewControl class
//

#include "pch.h"
#include "MultiViewControl.xaml.h"

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

//DependencyProperty^ scwin::MultiViewControl::contentTypeProperty = nullptr;
//
//void scwin::MultiViewControl::ContentPropertyChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ e)
//{
//
//}
//
//void scwin::MultiViewControl::RegisterDependencyProperties()
//{
//	if (contentTypeProperty == nullptr)
//	{
//		contentTypeProperty =
//			DependencyProperty::Register(
//				"ContentType",
//				Platform::Type::typeid,
//				MultiViewControl::typeid,
//				ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&ContentPropertyChanged))
//			);
//	}
//}

MultiViewControl::MultiViewControl()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::MultiViewControl::OnLoaded);
}


void scwin::MultiViewControl::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	MainFrame->Navigate(ContentType, nullptr);
}
