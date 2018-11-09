//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "scMainViewModel.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	private:
		scMainViewModel^ vm;

	public:
		MainPage(scMainViewModel^ vm, LandingContentStage stage);

	private:
		void InitSettings(LandingContentStage stage);

		property scMainViewModel^ ViewModel
		{
			scMainViewModel^ get();
		}

		void Frame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);

		void OnNavigationFailed(Platform::Object^ sender, NavigationFailedEventArgs^ e);
	};
}
