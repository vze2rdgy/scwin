#pragma once

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;

namespace scwin
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class LayoutAwarePage: public Page
	{
	private:
		//static DependencyProperty^ defaultViewModelProperty;
		Vector<Control^>^ layoutAwareControls;
		Windows::Foundation::EventRegistrationToken
			sizeChangedEventCookie,
			accelaratedKeyEventCookie,
			pointerPressedEventCookie;
		String^ pageKey;

	public:

		static void RegisterDependencyProperties();

		//static property DependencyProperty^ DefaultViewModelProperty
		//{
		//public:
		//	DependencyProperty^ get();
		//}

	private:
		void CoreDispatcher_AccelaratorKeyActivated(Windows::UI::Core::CoreDispatcher^ sender, Windows::UI::Core::AcceleratorKeyEventArgs^ eargs);

		// Invoke on every mouse click, touch screen tab or equivalent interaction when this page is
		// activated and occupies entire window. used to detect browser style next/prevous 
		// mouse button clicks to navigate between pages.
		void CoreWindow_PointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ e);

	protected:
		virtual String^ DetermineVisualState(double width);
		virtual void GoHome(Object^ sender, RoutedEventArgs^ args);
		virtual void GoBack(Object^ sender, RoutedEventArgs^ args);
		virtual void GoForward(Object^ sender, RoutedEventArgs^ args);
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	internal:
		virtual void LoadState(Object^ navigationParameter, Map<String^, Object^>^ pageState);
		virtual void SaveState(Map<String^, Object^>^ pageState);


	internal: // methods
		LayoutAwarePage();

		// called in Loaded event. Apply visual state management changes.
		void StartLayoutUpdates(Object^ sender, RoutedEventArgs^ eargs);

		// invoked by events to indicate that sender should stop receiving
		// visual state management changes
		void StopLayoutUpdates(Object^ sender, RoutedEventArgs^ e);

		// Update all controls that are listening for visual state changes with
		// the current visual state.
		void InvalidateVisualState();

	public: // Properties
		//property IObservableMap<String^, Object^>^ DefaultViewModel
		//{
		//public:
		//	IObservableMap<String^, Object^>^ get();
		//	void set(IObservableMap<String^, Object^>^);
		//}

	};

}