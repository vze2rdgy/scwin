#include "pch.h"
#include "UserInteractionStateTrigger.h"

using namespace Windows::UI::ViewManagement;

DependencyProperty^ UserInteractionStateTrigger::interactionMode = nullptr;

void scwin::UserInteractionStateTrigger::Register()
{
	if (interactionMode == nullptr)
	{
		interactionMode = DependencyProperty::Register("InteractionMode",
			UserInteractionMode::typeid,
			UserInteractionStateTrigger::typeid,
			ref new PropertyMetadata(Windows::UI::ViewManagement::UserInteractionMode::Mouse, ref new PropertyChangedCallback(&UserInteractionStateTrigger::OnInteractionModeChanged)));
	}
}

UserInteractionStateTrigger::UserInteractionStateTrigger()
{
	using namespace Windows::ApplicationModel;
	if (!DesignMode::DesignModeEnabled)
	{
		if (Window::Current != nullptr)
		{
			Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &UserInteractionStateTrigger::Window_SizeChanged);
			UpdateTrigger(InteractionMode);
		}
	}
}

void scwin::UserInteractionStateTrigger::UpdateTrigger(Windows::UI::ViewManagement::UserInteractionMode mode)
{
	IsActive = mode == UIViewSettings::GetForCurrentView()->UserInteractionMode;
}

void scwin::UserInteractionStateTrigger::Window_SizeChanged(Object ^ sender, WindowSizeChangedEventArgs ^ e)
{
	UpdateTrigger(InteractionMode);
}

void scwin::UserInteractionStateTrigger::OnInteractionModeChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ e)
{
	auto obj = (UserInteractionStateTrigger^)d;
	if (!Windows::ApplicationModel::DesignMode::DesignModeEnabled)
	{
		auto orientation = (UserInteractionMode)e->NewValue;
		obj->UpdateTrigger(orientation);
	}
}

DependencyProperty^ UserInteractionStateTrigger::InteractionModeProperty::get()
{
	return interactionMode;
}

Windows::UI::ViewManagement::UserInteractionMode  UserInteractionStateTrigger::InteractionMode::get()
{
	return (Windows::UI::ViewManagement::UserInteractionMode)GetValue(interactionMode);
}

void UserInteractionStateTrigger::InteractionMode::set(Windows::UI::ViewManagement::UserInteractionMode val)
{
	SetValue(interactionMode, val);
}

bool UserInteractionStateTrigger::IsActive::get()
{
	return isActive;
}

void UserInteractionStateTrigger::IsActive::set(bool val)
{
	isActive = val;
	SetActive(val);
	IsActiveChanged(this, val);
}