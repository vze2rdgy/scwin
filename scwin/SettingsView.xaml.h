//
// SettingsView.xaml.h
// Declaration of the SettingsView class
//

#pragma once

#include "SettingsView.g.h"
#include "StringToDataTemplateConverter.h"
#include "SettingsViewModel.h"
#include "ContentControlRevised.h"

namespace scwin
{

	ref class SettingsViewModel;

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SettingsView sealed
	{
	public:
		SettingsView();

		property SettingsViewModel^ ViewModel;

	private:
		void EditOrgButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void EditOtherDetailsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void rootPivot_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void hlinkSetupBusinessDetails_Click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
		void EditButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CancelButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnSignout_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};

}
