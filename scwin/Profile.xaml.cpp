//
// Profile.xaml.cpp
// Implementation of the Profile class
//

#include "pch.h"
#include "Profile.xaml.h"
#include "scMainViewModel.h"
#include "UserProfile.h"


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
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml::Media;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

Profile::Profile()
{
	InitializeComponent();
}



void scwin::Profile::UserProfileUpdate_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto up = ref new UserProfile();
	up->FullName = FullNameText->Text;
	this->ViewModel->UpdateUser(up);
}


void scwin::Profile::PasswordUpdate_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->ViewModel->ChangePassword(OldPassword->Password, NewPassword->Password);
}


void scwin::Profile::UserControl_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args)
{
	this->ViewModel = safe_cast<scMainViewModel^>(this->DataContext);
}


void scwin::Profile::Image_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	profilePictureSelect->IsOpen = true;
	return;

	auto picker = ref new FileOpenPicker();
	picker->ViewMode = PickerViewMode::Thumbnail;
	picker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	picker->FileTypeFilter->Append(L".png");
	picker->FileTypeFilter->Append(L".jpg");
	picker->FileTypeFilter->Append(L".jpeg");
	picker->FileTypeFilter->Append(L".gif");
	auto async = picker->PickSingleFileAsync();
	using namespace Windows::Storage::Streams;
	task<StorageFile^> tpickAsync(picker->PickSingleFileAsync());
	WeakReference wr(this);
	WeakReference wrs(sender);
	tpickAsync.then([wr, wrs](StorageFile^ file)
	{
		task<IRandomAccessStream^> topenfile(file->OpenAsync(FileAccessMode::Read));
		topenfile.then([wr, wrs](IRandomAccessStream^ stream)
		{
			auto bmp = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage();
			bmp->SetSource(stream);
			auto img = safe_cast<Image^>(wrs.Resolve<Image>());
			img->Source = bmp;
		}).then([wr]()
		{
			wr.Resolve<scwin::Profile>()->ViewModel->SetStatusMessage("Image is set.");
		});
	});
}
