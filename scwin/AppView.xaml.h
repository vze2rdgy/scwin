//
// AppView.xaml.h
// Declaration of the AppView class
//

#pragma once

using namespace Platform::Collections;
using namespace Windows::UI::Core;
using namespace Windows::Foundation::Numerics;

#include "scMainViewModel.h"
#include "IsTenFootTrigger.h"
#include "StreetcatAppTrigger.h"
#include "NavMenuListView.h"
#include "LayoutAwarePage.h"
#include "StatusMessageTrigger.h"
#include "CompositionShadow.xaml.h"
#include "AppView.g.h"

namespace scwin
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AppView sealed
	{
		static DependencyProperty^ slidePanelVisiblePositionProperty;
		static DependencyProperty^ slidePanelCollapsedPositionProperty;

	private:
		Rect togglePaneButtonRect;
		CoreCursor^ defCursor;


	public:

		static void Register();

		AppView(scMainViewModel^ vm);

		property Windows::UI::Xaml::Controls::Frame^ AppFrame
		{
			Windows::UI::Xaml::Controls::Frame^ get();
		}

		void OpenNavPane();

		property scMainViewModel^ ViewModel;

		static property DependencyProperty^ SlidePanelVisiblePositionProperty
		{
			DependencyProperty^ get() { return slidePanelVisiblePositionProperty; }
		}

		static property DependencyProperty^ SlidePanelCollapsedPositionProperty
		{
			DependencyProperty^ get() { return slidePanelCollapsedPositionProperty; }
		}

		property IBox<double>^ SlidePanelVisiblePosition
		{
			IBox<double>^ get();
		private:
			void set(IBox<double>^);
		}

		property IBox<double>^ SlidePanelCollapsedPosition
		{
			IBox<double>^  get();
		private:
			void set(IBox<double>^);
		}

	internal:
		/// <summary>
		/// An event to notify listeners when the hamburger button may occlude other content in the app.
		/// The custom "PageHeader" user control is using this.
		/// </summary>
		event TypedEventHandler<AppView^, Rect>^ TogglePaneButtonRectChanged;

		property Rect TogglePaneButtonRect
		{
			Rect get() 
			{ 
				return togglePaneButtonRect; 
			}
		private:
			void set(Rect value) 
			{ 
				togglePaneButtonRect = value; 
			}
		}


	private:
		void OnLoaded(Object^ sender, RoutedEventArgs^ e);
		void AppShell_KeyDown(Object^ sender, KeyRoutedEventArgs^ e);
		void SystemNavigationManager_BackRequested(Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);
		void NavMenuList_ItemInvoked(Object^ sender, ListViewItem^ e);
		void OnNavigatingToPage(Object^ sender, NavigatingCancelEventArgs^ e);
		void RootSplitView_PaneClosed(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args);
		void TogglePaneButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TogglePaneButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CheckTogglePaneButtonSizeChanged();
		void RootSplitViewDisplayModeChangedCallback(DependencyObject^ sender, DependencyProperty^ dp);
		void NavMenuItemContainerContentChanging(ListView^ sender, ContainerContentChangingEventArgs^ e);
		void frame_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void NavManager_BackRequested(Object^ sender, BackRequestedEventArgs^ e);
		void LogoutNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void SettingsNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e);
		void TextBoxSearch_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextBoxSearch_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectionChanged(Platform::Object ^sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^e);
		void btnRemoveMesage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void btnForNavBar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CartButton_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e);
		void OnWindowSizeChanged(Object^ sender, WindowSizeChangedEventArgs^ eargs);
		void UpdateSlidePanelPosition();
	};

}
