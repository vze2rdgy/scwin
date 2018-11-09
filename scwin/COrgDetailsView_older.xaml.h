//
// COrgDetailsView.xaml.h
// Declaration of the COrgDetailsView class
//

#pragma once

#include "COrgDetailsView_older.g.h"
#include "CustomerViewModel.h"


using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Media::Animation;


namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class COrgDetailsView sealed
	{
		Compositor^ compositor;
		std::vector<CostCenterDtosGroupedBindable^> costCenters;

	protected:
		void OnNavigatedTo(NavigationEventArgs^ e) override;
		void OnNavigatingFrom(NavigatingCancelEventArgs^ e) override;

	public:
		COrgDetailsView();

		property CustomerViewModel^ ViewModel;

	private:
		void btnEditOrg_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void lnkOtherLocations_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPreviousLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnPreviousLocation_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnNextLocation_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnNextLocation_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
	};
}
