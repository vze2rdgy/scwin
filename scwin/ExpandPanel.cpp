//
// ExpandPanel.cpp
// Implementation of the ExpandPanel class.
//

#include "pch.h"
#include "ExpandPanel.h"

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

DependencyProperty^ scwin::ExpandPanel::headerContentProperty = nullptr;
DependencyProperty^ scwin::ExpandPanel::isExpandedProperty = nullptr;
DependencyProperty^ scwin::ExpandPanel::cornerRadiusProperty = nullptr;

void scwin::ExpandPanel::RegisterDependencyProperties()
{
	if (headerContentProperty == nullptr)
		headerContentProperty = DependencyProperty::Register("HeaderContent", Object::typeid, ExpandPanel::typeid, nullptr);
	if (isExpandedProperty == nullptr)
		isExpandedProperty = DependencyProperty::Register("IsExpanded", bool::typeid, ExpandPanel::typeid, ref new PropertyMetadata(true));
	if (cornerRadiusProperty == nullptr)
		cornerRadiusProperty = DependencyProperty::Register("CornerRadius", CornerRadius::typeid, ExpandPanel::typeid, nullptr);
}

ExpandPanel::ExpandPanel()
{
	DefaultStyleKey = "scwin.ExpandPanel";
}

EventRegistrationToken ExpandPanel::Expanded::add(RoutedEventHandler^ h)
{
	return expandedEvent += h;
}

void ExpandPanel::Expanded::remove(EventRegistrationToken token)
{
	expandedEvent -= token;
}

void ExpandPanel::Expanded::raise(Object^ sender, RoutedEventArgs^ args)
{
	expandedEvent(sender, args);
}

void scwin::ExpandPanel::ChangeVisualState(bool useTransitions)
{
	if (IsExpanded)
	{
		if (contentElement)
		{
			contentElement->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		VisualStateManager::GoToState(this, "Expanded", useTransitions);
	}
	else
	{
		VisualStateManager::GoToState(this, "Collapsed", useTransitions);
		collapsedState = (Windows::UI::Xaml::VisualState^)GetTemplateChild("Collapsed");
		if (!collapsedState)
		{
			if (contentElement)
			{
				contentElement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			}
		}
	}
}

void scwin::ExpandPanel::OnApplyTemplate()
{
	Control::OnApplyTemplate();
	toggleExpander = (Windows::UI::Xaml::Controls::Primitives::ToggleButton^)GetTemplateChild("ExpandCollapseButton");
	if (toggleExpander)
	{
		toggleExpander->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &scwin::ExpandPanel::OnClick);
	}
	contentElement = (FrameworkElement^)GetTemplateChild("Content");
	if (contentElement)
	{
		collapsedState = (Windows::UI::Xaml::VisualState^)GetTemplateChild("Collapsed");
		if (collapsedState && collapsedState->Storyboard)
		{
			collapsedState->Storyboard->Completed += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &scwin::ExpandPanel::OnCompleted);
		}
	}
	ChangeVisualState(false);
}


void scwin::ExpandPanel::OnClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	IsExpanded = !IsExpanded;
	toggleExpander->IsChecked = IsExpanded;
	ChangeVisualState(useTransitions);
	expandedEvent(this, e);
}


void scwin::ExpandPanel::OnCompleted(Platform::Object ^sender, Platform::Object ^args)
{
	contentElement->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}
