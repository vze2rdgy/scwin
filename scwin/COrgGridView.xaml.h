//
// COrgGridView.xaml.h
// Declaration of the COrgGridView class
//

#pragma once

#include "CustomerDtosGroupedBindable.h"
#include "COrgGridView.g.h"
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
	public ref class COrgGridView sealed
	{
	private:
		Compositor^ compositor;
		CoreCursor^ origCursor;

	protected:
		void OnNavigatedTo(NavigationEventArgs^ e) override;
		void OnNavigatingFrom(NavigatingCancelEventArgs^ e) override;

	public:
		COrgGridView();

		property CustomerViewModel^ ViewModel;

	private:
		void gvOrgs_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnGridViewLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void gvOrgs_SearchKeyChanged(Windows::UI::Xaml::Controls::ListView^ sender, Platform::String^ args);
		void gvOrgs_AddButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
