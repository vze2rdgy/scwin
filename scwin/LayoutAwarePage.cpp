#include "pch.h"
#include "LayoutAwarePage.h"
#include "ObservableDictionary.h"
#include "SuspensionManager.h"

using namespace Sg;



//DependencyProperty^ LayoutAwarePage::defaultViewModelProperty = nullptr;

//https://docs.microsoft.com/en-us/windows/uwp/xaml-platform/custom-dependency-properties

void LayoutAwarePage::RegisterDependencyProperties()
{
	//if (defaultViewModelProperty == nullptr)
	//{
	//	defaultViewModelProperty =
	//		DependencyProperty::Register(
	//			"DefaultViewModel",
	//			IObservableMap<String^, Object^>::typeid,
	//			LayoutAwarePage::typeid,
	//			nullptr
	//		);
	//}

}

void scwin::LayoutAwarePage::CoreDispatcher_AccelaratorKeyActivated(Windows::UI::Core::CoreDispatcher ^ sender, Windows::UI::Core::AcceleratorKeyEventArgs ^ eargs)
{
	using namespace Windows::UI::Core;
	using namespace Windows::System;

	auto virtualKey = eargs->VirtualKey;
	if (
		(eargs->EventType == CoreAcceleratorKeyEventType::SystemKeyDown ||
			eargs->EventType == CoreAcceleratorKeyEventType::KeyDown) &&
			(virtualKey == VirtualKey::Left || virtualKey == VirtualKey::Right ||
		(int)virtualKey == 166 || (int)virtualKey == 167)
		)
	{
		auto coreWindow = Window::Current->CoreWindow;
		auto downState = CoreVirtualKeyStates::Down;
		bool menuKey = (coreWindow->GetKeyState(VirtualKey::Menu) & downState) == downState;
		bool controlKey = (coreWindow->GetKeyState(VirtualKey::Control) & downState) == downState;
		bool shiftKey = (coreWindow->GetKeyState(VirtualKey::Shift) & downState) == downState;
		bool noModifiers = !menuKey && !controlKey && !shiftKey;
		bool onlyAlt = menuKey && !controlKey && !shiftKey;
		if (((int)virtualKey == 166 && noModifiers) ||
			(virtualKey == VirtualKey::Left && onlyAlt))
		{
			eargs->Handled = true;
			GoBack(this, ref new RoutedEventArgs());
		}
		else if (((int)virtualKey == 167 && noModifiers) ||
			(virtualKey == VirtualKey::Right && onlyAlt))
		{
			eargs->Handled = true;
			GoForward(this, ref new RoutedEventArgs());
		}
	}
}

void scwin::LayoutAwarePage::CoreWindow_PointerPressed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ e)
{
	auto properties = e->CurrentPoint->Properties;
	if (properties->IsLeftButtonPressed || properties->IsRightButtonPressed || properties->IsMiddleButtonPressed)
	{
		return;
	}
	// if mouse back/forward is pressed
	bool backPressed = properties->IsXButton1Pressed;
	bool forwardPressed = properties->IsXButton2Pressed;
	if (backPressed ^ forwardPressed) // exclusive or 0^0 = 0, 1^0=1, 0^1=1, 1^1=0
	{
		// if either of the buttons are pressed.
		e->Handled = true;
		if (backPressed)
			this->GoBack(this, ref new RoutedEventArgs());
		if (forwardPressed)
			this->GoForward(this, ref new RoutedEventArgs());
	}
}

String ^ scwin::LayoutAwarePage::DetermineVisualState(double width)
{
	return (width < 768) ? "Below768Layout" : "DefaultLayout";
}

void scwin::LayoutAwarePage::GoHome(Object ^ sender, RoutedEventArgs ^ args)
{
	if (this->Frame != nullptr)
	{
		while (this->Frame->CanGoBack)
			this->Frame->GoBack();
	}
}

void scwin::LayoutAwarePage::GoBack(Object ^ sender, RoutedEventArgs ^ args)
{
	if (this->Frame != nullptr && this->Frame->CanGoBack)
		this->Frame->GoBack();
}

void scwin::LayoutAwarePage::GoForward(Object ^ sender, RoutedEventArgs ^ args)
{
	if (this->Frame != nullptr && this->Frame->CanGoForward)
		this->Frame->GoForward();
}

void scwin::LayoutAwarePage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	using namespace Windows::UI::Xaml::Navigation;

	if (pageKey != nullptr)
		return;
	auto frameState = SuspensionManager::SessionStateForFrame(this->Frame);
	this->pageKey = L"Page-" + this->Frame->BackStackDepth;
	if (e->NavigationMode == NavigationMode::New)
	{
		// clear existing state for forward navigation when adding a new page to the 
		// navigation stack
		auto nextPageKey = this->pageKey;
		int nextPageIndex = this->Frame->BackStackDepth;
		while (frameState->HasKey(nextPageKey))
		{
			frameState->Remove(nextPageKey);
			nextPageIndex++;
			nextPageKey = L"Page" + nextPageIndex;
		}
		this->LoadState(e->Parameter, nullptr);
	}
	else
	{
		this->LoadState(e->Parameter, (Map<String^, Object^>^)frameState->Lookup(pageKey));
	}
}

void scwin::LayoutAwarePage::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	if (pageKey != nullptr)
	{
		auto frameState = SuspensionManager::SessionStateForFrame(this->Frame);
		auto pageState = ref new Map<String^, Object^>();
		SaveState(pageState);
		if (frameState->HasKey(pageKey))
		{
			frameState->Remove(pageKey);
		}
		frameState->Insert(pageKey, pageState);
	}
}

void scwin::LayoutAwarePage::LoadState(Object ^ navigationParameter, Map<String^, Object^>^ pageState)
{
}

void scwin::LayoutAwarePage::SaveState(Map<String^, Object^>^ pageState)
{
}

LayoutAwarePage::LayoutAwarePage()
{
	using namespace Windows::Foundation;
	using namespace Windows::UI::Core;

	if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
	{
		return;
	}

	// when the page is loaded into the visual tree, map 
	// a. application view state to the visual state of this page
	// b. handle keyboard and mouse navigation requests.
	this->Loaded += ref new RoutedEventHandler([this](Object^ sender, RoutedEventArgs^ eargs)
	{
		this->StartLayoutUpdates(sender, eargs);
		// keyboard/mouse navigation is applied only when entire window is occuped by the page
		if (ActualHeight == Window::Current->Bounds.Height &&
			ActualWidth == Window::Current->Bounds.Width)
		{
			// list to window directly so focus isn't required
			accelaratedKeyEventCookie = Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated +=
				ref new TypedEventHandler<CoreDispatcher^, AcceleratorKeyEventArgs^>(this, &LayoutAwarePage::CoreDispatcher_AccelaratorKeyActivated);
			pointerPressedEventCookie = Window::Current->CoreWindow->PointerPressed +=
				ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &LayoutAwarePage::CoreWindow_PointerPressed);
		}
	});

	this->Unloaded += ref new RoutedEventHandler([this](Object^ sender, RoutedEventArgs^ e)
	{
		this->StopLayoutUpdates(sender, e);
		Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated -= accelaratedKeyEventCookie;
		Window::Current->CoreWindow->PointerPressed -= pointerPressedEventCookie;
	});
}




void scwin::LayoutAwarePage::StartLayoutUpdates(Object ^ sender, RoutedEventArgs ^ eargs)
{
	auto control = dynamic_cast<Control^>(sender);
	if (control == nullptr) return;
	if (this->layoutAwareControls == nullptr)
	{
		sizeChangedEventCookie = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler([this](Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ eargs)
		{
			this->InvalidateVisualState();
		});
	}
}

void scwin::LayoutAwarePage::StopLayoutUpdates(Object ^ sender, RoutedEventArgs ^ e)
{
	auto control = dynamic_cast<Control^>(sender);
	if (control != nullptr && layoutAwareControls != nullptr)
	{
		UINT index;
		if (layoutAwareControls->IndexOf(control, &index))
		{
			layoutAwareControls->RemoveAt(index);
		}
		if (!layoutAwareControls->Size)
		{
			layoutAwareControls = nullptr;
			Window::Current->SizeChanged -= sizeChangedEventCookie;
		}
	}
}

void scwin::LayoutAwarePage::InvalidateVisualState()
{
	if (this->layoutAwareControls != nullptr)
	{
		String^ visualState = DetermineVisualState(Window::Current->Bounds.Width);
		for (auto control : layoutAwareControls)
		{
			VisualStateManager::GoToState(control, visualState, false);
		}
	}
}

//DependencyProperty^ LayoutAwarePage::DefaultViewModelProperty::get()
//{
//	return defaultViewModelProperty;
//}
//
//IObservableMap<String^, Object^>^ LayoutAwarePage::DefaultViewModel::get()
//{
//	return (IObservableMap<String^, Object^>^)GetValue(DefaultViewModelProperty);
//}
//
//void LayoutAwarePage::DefaultViewModel::set(IObservableMap<String^, Object^>^ val)
//{
//	SetValue(DefaultViewModelProperty, val);
//}