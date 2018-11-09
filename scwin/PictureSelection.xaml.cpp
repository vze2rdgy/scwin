//
// PictureSelection.xaml.cpp
// Implementation of the PictureSelection class
//

#include "pch.h"
#include "PictureSelection.xaml.h"

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

using namespace Windows::UI::Popups;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

PictureSelection::PictureSelection()
{
	InitializeComponent();
}


void scwin::PictureSelection::btnDelete_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	MessageDialog^ dlg = ref new MessageDialog("Do you want to delete picture ?", "Delete Picture");
	WeakReference wr(this);
	auto cmdYes = ref new UICommand(
		"Yes",
		ref new UICommandInvokedHandler([wr](IUICommand^ cmd)
		{

		})
	);
	auto cmdNo = ref new UICommand(
		"No",
		ref new UICommandInvokedHandler([wr](IUICommand^ cmd)
	{

	})
	);
	dlg->Commands->Append(cmdYes);
	dlg->Commands->Append(cmdNo);
	dlg->CancelCommandIndex = 1;
	dlg->DefaultCommandIndex = 1;
	dlg->ShowAsync();
}


void scwin::PictureSelection::btnClose_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto popups = Windows::UI::Xaml::Media::VisualTreeHelper::GetOpenPopups(Window::Current);
	for (auto p : popups)
	{
		if (p->Name == L"profilePictureSelect")
		{
			p->IsOpen = false;
		}
	}
}


void scwin::PictureSelection::btnAccept_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}
