//
// SettingsView.xaml.cpp
// Implementation of the SettingsView class
//

#include "pch.h"
#include "SettingsView.xaml.h"
#include "UpdateOrg.xaml.h"

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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

SettingsView::SettingsView()
{
	ViewModel = ref new SettingsViewModel();
	auto converter = ref new StringToDataTemplateConverter(Resources);
	Resources->Insert("StringToDataTemplateConverter", converter);
	InitializeComponent();
}


void scwin::SettingsView::EditOrgButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!splitView->IsPaneOpen)
	{
		ViewModel->SelectPanelContent(0);
		splitView->IsPaneOpen = true;
	}
}


void scwin::SettingsView::EditOtherDetailsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!splitView->IsPaneOpen)
	{
		ViewModel->SelectPanelContent(1);
		splitView->IsPaneOpen = true;
	}
}


void scwin::SettingsView::rootPivot_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	ViewModel->LoadState(rootPivot->SelectedIndex);
}


void scwin::SettingsView::hlinkSetupBusinessDetails_Click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args)
{
	if (!splitView->IsPaneOpen)
	{
		ViewModel->SelectPanelContent(0);
		splitView->IsPaneOpen = true;
	}
}


void scwin::SettingsView::EditButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	create_task(ViewModel->SaveAsync())
		.then([=](bool result)
	{
		if (result)
		{
			splitView->IsPaneOpen = false;
		}
	});
}


void scwin::SettingsView::CancelButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	create_task(ViewModel->CancelAsync())
		.then([=](bool result)
	{
		if (result)
		{
			splitView->IsPaneOpen = false;
		}
	});
}


void scwin::SettingsView::btnSignout_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	using namespace Windows::UI::Popups;
	MessageDialog^ dlg = ref new MessageDialog(L"Are you sure you want to signout?", L"Signout");
	dlg->Commands->Append(ref new UICommand("Yes"));
	dlg->Commands->Append(ref new UICommand("No"));
	dlg->CancelCommandIndex = 1;
	dlg->DefaultCommandIndex = 1;
	task<IUICommand^> showTask(dlg->ShowAsync());
	showTask.then([=](IUICommand^ cmd)
	{
		if (cmd->Label == L"Yes")
		{
			scMainViewModel::Instance->Logout();
		}
	});
}
