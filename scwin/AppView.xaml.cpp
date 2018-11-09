//
// AppView.xaml.cpp
// Implementation of the AppView class
//

#include "pch.h"
#include "MainDashboard.xaml.h"
#include "Customers.xaml.h"
#include "Products.xaml.h"
#include "Administration.xaml.h"
#include "Orders.xaml.h"
#include "NavMenuListView.h"
#include "AppView.xaml.h"
#include "Account.xaml.h"
#include "MultiViewControl.xaml.h"
#include "StatusMessageTemplateSelector.h"
#include <sstream>

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

DependencyProperty^ AppView::slidePanelVisiblePositionProperty = nullptr;
DependencyProperty^ AppView::slidePanelCollapsedPositionProperty = nullptr;

void scwin::AppView::Register()
{
	if (slidePanelVisiblePositionProperty == nullptr)
	{
		slidePanelVisiblePositionProperty = DependencyProperty::Register(
			PROPERTYNAME(SlidePanelVisiblePosition),
			IBox<double>::typeid,
			AppView::typeid,
			ref new PropertyMetadata(0.)
		);
	}
	if (slidePanelCollapsedPositionProperty == nullptr)
	{
		slidePanelCollapsedPositionProperty = DependencyProperty::Register(
			PROPERTYNAME(SlidePanelCollapsedPosition),
			IBox<double>::typeid,
			AppView::typeid,
			ref new PropertyMetadata(0.)
		);
	}
}

AppView::AppView(scMainViewModel^ vm)
{

	auto selector = ref new StatusMessageTemplateSelector(Resources);
	Resources->Insert(L"StatusMessageTemplateSelector", selector);


	this->DataContext = vm;
	this->ViewModel = vm;
	if (vm)
	{
		vm->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler(this, &scwin::AppView::OnPropertyChanged);
	}

	InitializeComponent();

	defCursor = Window::Current->CoreWindow->PointerCursor;

	Loaded += ref new RoutedEventHandler(this, &AppView::OnLoaded);
	RootSplitView->LightDismissOverlayMode = LightDismissOverlayMode::On;

	RootSplitView->RegisterPropertyChangedCallback(
		SplitView::DisplayModeProperty,
		ref new DependencyPropertyChangedCallback(this, &AppView::RootSplitViewDisplayModeChangedCallback)
	);

	Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->
		BackRequested += ref new EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(this, &AppView::SystemNavigationManager_BackRequested);
	auto navm = SystemNavigationManager::GetForCurrentView();
	navm->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Visible;
	navm->BackRequested += ref new EventHandler<BackRequestedEventArgs^>(this, &AppView::NavManager_BackRequested);
	NavigationCacheMode = Windows::UI::Xaml::Navigation::NavigationCacheMode::Required;

	NavMenuList->SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, &scwin::AppView::OnSelectionChanged);

	Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &AppView::OnWindowSizeChanged);


}



Frame^ AppView::AppFrame::get()
{
	return frame;
}


void scwin::AppView::OpenNavPane()
{
}

void scwin::AppView::OnLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
	//TogglePaneButton->Focus(Xaml::FocusState::Programmatic);
	//TODO capture what user selected last time before he closed the app.
	NavMenuList->SelectedIndex = 0;
	//UpdateSlidePanelPosition();

}

void scwin::AppView::AppShell_KeyDown(Object ^ sender, KeyRoutedEventArgs ^ e)
{
	FocusNavigationDirection direction = FocusNavigationDirection::None;
	switch (e->Key)
	{
	case Windows::System::VirtualKey::Left:
	case Windows::System::VirtualKey::GamepadDPadLeft:
	case Windows::System::VirtualKey::GamepadLeftThumbstickLeft:
	case Windows::System::VirtualKey::NavigationLeft:
		direction = FocusNavigationDirection::Left;
		break;
	case Windows::System::VirtualKey::Right:
	case Windows::System::VirtualKey::GamepadDPadRight:
	case Windows::System::VirtualKey::GamepadLeftThumbstickRight:
	case Windows::System::VirtualKey::NavigationRight:
		direction = FocusNavigationDirection::Right;
		break;

	case Windows::System::VirtualKey::Up:
	case Windows::System::VirtualKey::GamepadDPadUp:
	case Windows::System::VirtualKey::GamepadLeftThumbstickUp:
	case Windows::System::VirtualKey::NavigationUp:
		direction = FocusNavigationDirection::Up;
		break;

	case Windows::System::VirtualKey::Down:
	case Windows::System::VirtualKey::GamepadDPadDown:
	case Windows::System::VirtualKey::GamepadLeftThumbstickDown:
	case Windows::System::VirtualKey::NavigationDown:
		direction = FocusNavigationDirection::Down;
		break;
	}

	if (direction != FocusNavigationDirection::None)
	{
		Control^ control = dynamic_cast<Control^>(FocusManager::FindNextFocusableElement(direction));
		if (control != nullptr)
		{
			control->Focus(Windows::UI::Xaml::FocusState::Programmatic);
		}
	}
}

void scwin::AppView::OnNavigatingToPage(Object ^ sender, NavigatingCancelEventArgs ^ e)
{
	if (e->NavigationMode == NavigationMode::Back)
	{
		Vector<NavMenuItem^>^ navlist = (Vector<NavMenuItem^>^)NavMenuList->ItemsSource;
		NavMenuItem^ item = nullptr;
		for (auto p : navlist)
		{
			if (p->DestPage.Name == e->SourcePageType.Name)
			{
				item = p;
				break;
			}
		}

		if (item == nullptr && AppFrame->BackStackDepth > 0)
		{
			// In cases where a page drills into sub-pages then we'll highlight the most recent
			// navigation menu item that appears in the BackStack
			for (auto entry : AppFrame->BackStack)
			{
				Object^ p = nullptr;
				for (auto p : navlist)
				{
					if (p->DestPage.Name == entry->SourcePageType.Name)
					{
						item = p;
						break;
					}
				}
			}
		}

		if (item == nullptr)
		{
			// TODO: check if the e->SourcePageType.Name is Accounts and if so highlight accounts buttons
		}

		for (NavMenuItem^ i : navlist)
		{
			i->IsSelected = false;
		}
		if (item != nullptr)
		{
			item->IsSelected = true;
		}

		auto container = (ListViewItem^)NavMenuList->ContainerFromItem(item);

		// While updating the selection state of the item prevent it from taking keyboard focus.  If a
		// user is invoking the back button via the keyboard causing the selected nav menu item to change
		// then focus will remain on the back button.

		if (container != nullptr) container->IsTabStop = false;
		NavMenuList->SetSelectedItem(container);
		if (container != nullptr) container->IsTabStop = true;
	}

}

void scwin::AppView::TogglePaneButton_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	auto btn = (FrameworkElement^)sender;
	auto attachedFlyout = FlyoutBase::GetAttachedFlyout(btn);
	attachedFlyout->Hide();

}

void scwin::AppView::CheckTogglePaneButtonSizeChanged()
{
	if (RootSplitView->DisplayMode == SplitViewDisplayMode::Inline ||
		RootSplitView->DisplayMode == SplitViewDisplayMode::Overlay)
	{
		auto transform = btnForNavBar->TransformToVisual(this);
		auto rect = transform->TransformBounds(Rect(0, 0, (float)btnForNavBar->ActualWidth, (float)btnForNavBar->ActualHeight));
		togglePaneButtonRect = rect;
	}
	else
	{
		togglePaneButtonRect = Rect();
	}

	TogglePaneButtonRectChanged(this, TogglePaneButtonRect);
}


void scwin::AppView::TogglePaneButton_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	auto btn = (FrameworkElement^)sender;
	auto attachedFlyout = FlyoutBase::GetAttachedFlyout(btn);
	attachedFlyout->ShowAt(btn);

	CheckTogglePaneButtonSizeChanged();
}


void scwin::AppView::RootSplitView_PaneClosed(Windows::UI::Xaml::Controls::SplitView^ sender, Platform::Object^ args)
{
	CheckTogglePaneButtonSizeChanged();

}

void scwin::AppView::SystemNavigationManager_BackRequested(Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ e)
{
	if (!e->Handled && AppFrame->CanGoBack)
	{
		e->Handled = true;
		AppFrame->GoBack();
	}
}

void scwin::AppView::RootSplitViewDisplayModeChangedCallback(DependencyObject ^ sender, DependencyProperty ^ dp)
{
	CheckTogglePaneButtonSizeChanged();
}

void scwin::AppView::NavMenuItemContainerContentChanging(ListView ^ sender, ContainerContentChangingEventArgs ^ e)
{
	if (!e->InRecycleQueue && e->Item != nullptr &&
		dynamic_cast<NavMenuItem^>(e->Item) != nullptr)
	{
		e->ItemContainer->SetValue(
			Automation::AutomationProperties::NameProperty, ((NavMenuItem^)e->Item)->Label
		);
	}
	else
	{
		e->ItemContainer->ClearValue(Automation::AutomationProperties::NameProperty);
	}
}

void scwin::AppView::NavMenuList_ItemInvoked(Object ^ sender, ListViewItem ^ listviewitem)
{
	if (!listviewitem)
		return;

	Vector<NavMenuItem^>^ navlist = (Vector<NavMenuItem^>^)NavMenuList->ItemsSource;

	for (NavMenuItem^ i : navlist)
	{
		i->IsSelected = false;
	}
	auto item = (NavMenuItem^)((NavMenuListView^)(sender))->ItemFromContainer(listviewitem);
	if (item != nullptr)
	{
		item->IsSelected = true;
		if (item->DestPage.Name != AppFrame->CurrentSourcePageType.Name)
		{
			AppFrame->Navigate(item->DestPage, item->Arguments);
		}
	}
}

void scwin::AppView::OnSelectionChanged(Platform::Object ^sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^e)
{
	if (e->AddedItems->Size)
	{
		auto navItem = (NavMenuItem^)e->AddedItems->GetAt(0);
		navItem->IsSelected = true;
		if (navItem->DestPage.Name != AppFrame->CurrentSourcePageType.Name)
		{
			AppFrame->Navigate(navItem->DestPage, navItem->Arguments);
		}
	}
}



void scwin::AppView::frame_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}

void scwin::AppView::NavManager_BackRequested(Object ^ sender, BackRequestedEventArgs ^ e)
{
	// if handled, set e-Handled=true
}


void scwin::AppView::LogoutNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
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
			ViewModel->Logout();
		}
	});
}


void scwin::AppView::SettingsNavPaneButton_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	Vector<NavMenuItem^>^ navlist = (Vector<NavMenuItem^>^)NavMenuList->ItemsSource;
	for (NavMenuItem^ i : navlist)
	{
		i->IsSelected = false;
	}
	AppFrame->Navigate(TypeName(Account::typeid));
}


void scwin::AppView::OnPropertyChanged(Platform::Object ^sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^e)
{
	if (e->PropertyName == L"DisplayProgress")
	{
		if (ViewModel->DisplayProgress == Windows::UI::Xaml::Visibility::Visible)
		{
			if (Window::Current)
			{
				Window::Current->CoreWindow->PointerCursor = ref new
					Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Wait, 10);
			}
			btnForNavBar->IsEnabled = false;
			RootSplitView->IsEnabled = false;
		}
		else 
		{
			if (Window::Current)
			{
				Window::Current->CoreWindow->PointerCursor = defCursor;
			}
			RootSplitView->IsEnabled = true;
			btnForNavBar->IsEnabled = true;
		}
	}
}


void scwin::AppView::TextBoxSearch_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (TextBoxSearch->Text == SEARCHPROMPT)
	{
		TextBoxSearch->Text = L"";
	}
	else
	{
		TextBoxSearch->SelectAll();
	}
}


void scwin::AppView::TextBoxSearch_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (StringHelpers::IsNullOrEmpty(TextBoxSearch->Text))
	{
		TextBoxSearch->Text = SEARCHPROMPT;
	}
}




void scwin::AppView::btnRemoveMesage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (ViewModel->StatusMessages != nullptr)
	{
		auto toremove = dynamic_cast<StatusMessage^>(((Control^)sender)->DataContext);
		auto v = to_vector(ViewModel->StatusMessages);
		auto pos = find(v.cbegin(), v.cend(), toremove);
		if (pos != v.cend())
		{
			ViewModel->StatusMessages->RemoveAt(pos - v.cbegin());
		}
	}
}



void scwin::AppView::btnForNavBar_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (appNavBar->Visibility != Windows::UI::Xaml::Visibility::Visible)
	{
		appNavBar->Visibility = Windows::UI::Xaml::Visibility::Visible;
		appNavBar->Focus(Windows::UI::Xaml::FocusState::Programmatic);
	}
	else
		appNavBar->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

IBox<double>^ AppView::SlidePanelVisiblePosition::get()
{
	auto ret = (IBox<double>^)GetValue(slidePanelVisiblePositionProperty);
	return ret;
}

void AppView::SlidePanelVisiblePosition::set(IBox<double>^ val)
{
	SetValue(slidePanelVisiblePositionProperty, val);
}

IBox<double>^ AppView::SlidePanelCollapsedPosition::get()
{
	auto ret = (IBox<double>^)GetValue(slidePanelCollapsedPositionProperty);
	return ret;
}

void AppView::SlidePanelCollapsedPosition::set(IBox<double>^ val)
{
	SetValue(slidePanelCollapsedPositionProperty, val);
}


void scwin::AppView::CartButton_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e)
{
	// adjust hieight position.

	UpdateSlidePanelPosition();
	if (PanelSideLoad->Visibility == Windows::UI::Xaml::Visibility::Collapsed)
	{
		GOTOVIEWSTATE(this, "SlideFrameIntoView");
	}
	else
	{ 
		GOTOVIEWSTATE(this, "SlideFrameOutofView");
	}

}

void scwin::AppView::OnWindowSizeChanged(Object ^ sender, WindowSizeChangedEventArgs ^ eargs)
{
	// adjust hieight position.
	//UpdateSlidePanelPosition();
}

void scwin::AppView::UpdateSlidePanelPosition()
{
	auto dispInfo = DisplayInformation::GetForCurrentView();
	auto scaleFactor = dispInfo->RawPixelsPerViewPixel;
	auto rawDpi = dispInfo->RawDpiX;
	auto rscale = dispInfo->ResolutionScale;
	auto logicalscale = dispInfo->LogicalDpi;
	auto adjusted = LayoutRoot->ActualWidth;
	if (isnan(adjusted))
		return;
	auto panelWidth = adjusted * .3;
	auto defPanelWidth = PanelSideLoad->ActualWidth;
	PanelSideLoad->Width = panelWidth;
	double x, y;
	x = (scaleFactor * (adjusted - panelWidth));
	y = adjusted + 100.;
	SlidePanelVisiblePosition = x  ;
	SlidePanelCollapsedPosition = y;
}
