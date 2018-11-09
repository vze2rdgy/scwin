//
// NavMenuListView.cpp
// Implementation of the NavMenuListView class.
//

#include "pch.h"
#include "NavMenuListView.h"

using namespace scwin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

// The Templated Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234235

NavMenuListView::NavMenuListView() : ListView()
{
	SelectionMode = ListViewSelectionMode::Single;
	SingleSelectionFollowsFocus = false;
	IsItemClickEnabled = true;
	ItemClick += ref new ItemClickEventHandler(this, &NavMenuListView::ItemClickHandler);
	//SelectionChanged += ref new SelectionChangedEventHandler(this, &NavMenuListView::SelectionChangedHandler);
	this->Loaded += ref new RoutedEventHandler(this, &NavMenuListView::OnLoaded);
}

void scwin::NavMenuListView::OnLoaded(Object ^ sender, RoutedEventArgs ^ e)
{
	auto parent = VisualTreeHelper::GetParent(this);
	while (parent != nullptr && dynamic_cast<SplitView^>(parent) == nullptr)
	{
		parent = VisualTreeHelper::GetParent((DependencyObject^)parent);
	}
	if (parent != nullptr)
	{
		splitViewHost = dynamic_cast<SplitView^>(parent);
		splitViewHost->RegisterPropertyChangedCallback(
			SplitView::IsPaneOpenProperty,
			ref new DependencyPropertyChangedCallback(this, &NavMenuListView::IsOpenPanePropertyChangedCallback)
		);
		splitViewHost->RegisterPropertyChangedCallback(
			SplitView::DisplayModeProperty,
			ref new DependencyPropertyChangedCallback(this, &NavMenuListView::DisplayModePropertyChangedCallback)
		);
		OnPaneToggled();
	}
}


void scwin::NavMenuListView::SetSelectedItem(ListViewItem ^ item)
{
	int index = -1;
	if (item != nullptr)
	{
		index = IndexFromContainer(item);
	}
	for (int i = 0; i < (int)Items->Size; i++)
	{
		auto lvi = (ListViewItem^)ContainerFromIndex(i);
		if (i != index)
		{
			lvi->IsSelected = false;
		}
		else if (i == index)
		{
			lvi->IsSelected = true;
		}
	}
}

void scwin::NavMenuListView::OnApplyTemplate()
{
	using namespace Windows::UI::Xaml::Media::Animation;

	ListView::OnApplyTemplate();
	//remove entrance animation on the item containers.
	for (int i = 0; i < (int)ItemContainerTransitions->Size; i++)
	{
		if (dynamic_cast<EntranceThemeTransition^>(ItemContainerTransitions->GetAt(i)))
		{
			ItemContainerTransitions->RemoveAt(i);
		}
	}
}

void scwin::NavMenuListView::OnKeyDown(KeyRoutedEventArgs ^ e)
{
	using namespace Windows::UI::Core;
	using namespace Windows::System;

	auto focusedItem = FocusManager::GetFocusedElement();
	auto shiftKeyState = CoreWindow::GetForCurrentThread()->GetKeyState(VirtualKey::Shift);
	auto shiftKeyDown = (shiftKeyState & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;
	switch (e->Key)
	{
	case VirtualKey::Up:
		TryMoveFocus(FocusNavigationDirection::Up);
		e->Handled = true;
		break;
	case VirtualKey::Down:
		TryMoveFocus(FocusNavigationDirection::Down);
		e->Handled = true;
		break;
	case VirtualKey::Space:
	case VirtualKey::Enter:
		// enter and space will activate the action behnd selected item
		InvokeItem(focusedItem);
		e->Handled = true;
		break;
	default:
		ListView::OnKeyDown(e);
		break;
	}
}

void scwin::NavMenuListView::SelectionChangedHandler(Object ^ sender, SelectionChangedEventArgs ^ e)
{
	auto item = ContainerFromItem(e->AddedItems->First());
	InvokeItem(item);
}

void scwin::NavMenuListView::ItemClickHandler(Object ^ sender, ItemClickEventArgs ^ e)
{
	// triggered with a touch or mouse click ocurred
	auto item = ContainerFromItem(e->ClickedItem);
	InvokeItem(item);
}

void scwin::NavMenuListView::InvokeItem(Object^ focusedItem)
{
	auto item = dynamic_cast<ListViewItem^>(focusedItem);
	SetSelectedItem(item);
	ItemInvoked(this, item);
	if (splitViewHost->IsPaneOpen && (splitViewHost->DisplayMode == SplitViewDisplayMode::CompactOverlay ||
		splitViewHost->DisplayMode == SplitViewDisplayMode::Overlay))
	{
		splitViewHost->IsPaneOpen = false;
	}
	if (item != nullptr)
	{
		item->Focus(Xaml::FocusState::Programmatic);
	}
}

void scwin::NavMenuListView::OnPaneToggled()
{
	_ASSERT(this->ItemsPanelRoot != nullptr);

	if (ItemsPanelRoot)
	{
		if (splitViewHost->IsPaneOpen)
		{
			ItemsPanelRoot->ClearValue(FrameworkElement::WidthProperty);
			ItemsPanelRoot->ClearValue(FrameworkElement::HorizontalAlignmentProperty);
		}
		else if (splitViewHost->DisplayMode == SplitViewDisplayMode::CompactInline ||
			splitViewHost->DisplayMode == SplitViewDisplayMode::CompactOverlay)
		{
			ItemsPanelRoot->SetValue(FrameworkElement::WidthProperty, splitViewHost->CompactPaneLength);
			ItemsPanelRoot->SetValue(FrameworkElement::HorizontalAlignmentProperty, Windows::UI::Xaml::HorizontalAlignment::Left);
		}
	}
}

void scwin::NavMenuListView::IsOpenPanePropertyChangedCallback(DependencyObject ^ sender, DependencyProperty ^ args)
{
	OnPaneToggled();
}

void scwin::NavMenuListView::DisplayModePropertyChangedCallback(DependencyObject ^ sender, DependencyProperty ^ args)
{
	OnPaneToggled();
}

void scwin::NavMenuListView::TryMoveFocus(FocusNavigationDirection direction)
{
	if (direction == FocusNavigationDirection::Next || direction == FocusNavigationDirection::Previous)
	{
		FocusManager::TryMoveFocus(direction);
	}
	else
	{
		auto control = dynamic_cast<Control^>(FocusManager::FindNextFocusableElement(direction));
		if (control != nullptr)
		{
			control->Focus(Xaml::FocusState::Programmatic);
		}
	}
}
