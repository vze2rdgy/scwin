//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "SignUp.xaml.h"
#include "SignIn.xaml.h"
#include "MainPage.xaml.h"
#include "WaitView.xaml.h"

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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage(scMainViewModel^ vm, LandingContentStage stage)
{
	InitializeComponent();
	this->vm = vm;
	this->DataContext = vm;
	InitSettings(stage);
}

void scwin::MainPage::InitSettings(LandingContentStage stage)
{
	ApplicationData::Current->SetVersionAsync(1, ref new ApplicationDataSetVersionHandler([](SetVersionRequest^ req)
	{
	}));
	// check previous login status and navigate to either signup, signin or dashboad.
	switch (stage)
	{
	case LandingContentStage::SignUp:
		mainContent->Content = ref new SignUp(vm);
		break;
	case LandingContentStage::SignIn:
		mainContent->Content = ref new SignIn(vm);
		break;
	case LandingContentStage::Wait:
		mainContent->Content = ref new WaitView();
		break;
	default:
		throw Exception::CreateException(E_FAIL, "Failed to reach any content.");
	}
}

scMainViewModel^ MainPage::ViewModel::get()
{
	return vm;
}

void scwin::MainPage::Frame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{

}

void scwin::MainPage::OnNavigationFailed(Platform::Object ^ sender, NavigationFailedEventArgs ^ e)
{
	//throw ref new Exception(e->Exception->)
}
