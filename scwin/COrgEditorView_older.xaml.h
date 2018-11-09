//
// COrgEditorView.xaml.h
// Declaration of the COrgEditorView class
//

#pragma once

#include "CustomerViewModel.h"
#include "COrgEditorView_older.g.h"

using namespace Windows::UI::Composition;


namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Bindable ]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class COrgEditorView sealed : INotifyPropertyChanged
	{
		//bool islandscape = true;
		//EventRegistrationToken dispOrientationTok;
		Compositor^ compositor;


	public:
		COrgEditorView();

		property CustomerViewModel^ ViewModel;

		//property bool IsLandscape
		//{
		//	bool get() { return islandscape; }
		//}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

	protected:
		void OnNavigatedTo(NavigationEventArgs^ e) override;
		void OnNavigatingFrom(NavigatingCancelEventArgs^ e) override;

	private:
		void LogoImage_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void LogoImage_PointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void btnAddAddress_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void MenuItemOnTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e);


		//void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args);
		//void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		//void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		//void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);
		void cmbAddressSameAsOptions_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void cmbAddressSameAsOptions_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cmbAddressSameAsOptions_DataContextChanged(Windows::UI::Xaml::FrameworkElement^ sender, Windows::UI::Xaml::DataContextChangedEventArgs^ args);
		void cmbAddressSameAsOptions_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void btnDeleteAddress_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		void btnAddContact_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnCancelOrg_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnSaveOrg_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnRemoveContact_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnDeleteCostCenter_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnAddCostcenter_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void btnAddAddress_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MenuItemOnClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
