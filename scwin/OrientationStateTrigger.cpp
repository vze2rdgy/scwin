#include "pch.h"
#include "OrientationStateTrigger.h"


DependencyProperty^ OrientationStateTrigger::orientationProperty = nullptr;

void scwin::OrientationStateTrigger::RegisterDps()
{
	orientationProperty = DependencyProperty::Register("Orientation",
		Orientations::typeid, OrientationStateTrigger::typeid,
		ref new PropertyMetadata(Orientations::None,
			ref new PropertyChangedCallback(&OrientationStateTrigger::OnOrientationPropertyChanged)));
}

scwin::OrientationStateTrigger::OrientationStateTrigger()
{
	if (!Windows::ApplicationModel::DesignMode::DesignModeEnabled)
	{
		tok = DisplayInformation::GetForCurrentView()->OrientationChanged += 
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &OrientationStateTrigger::OrientationStateTrigger_OrientationChanged);
		UpdateTrigger(DisplayInformation::GetForCurrentView()->CurrentOrientation);
	}
}


scwin::OrientationStateTrigger::~OrientationStateTrigger()
{
	DisplayInformation::GetForCurrentView()->OrientationChanged -= tok;
}

void scwin::OrientationStateTrigger::OnOrientationPropertyChanged(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e)
{
	auto obj = (OrientationStateTrigger^)d;
	if (!Windows::ApplicationModel::DesignMode::DesignModeEnabled)
	{
		auto orientation = DisplayInformation::GetForCurrentView()->CurrentOrientation;
		obj->UpdateTrigger(orientation);
	}
}

void scwin::OrientationStateTrigger::UpdateTrigger(DisplayOrientations orientation)
{
	if (orientation == DisplayOrientations::None)
	{
		IsActive = false;
	}
	else if (orientation == DisplayOrientations::Landscape ||
		orientation == DisplayOrientations::LandscapeFlipped)
	{
		IsActive = Orientation == Orientations::Landscape;
	}
	else if (orientation == DisplayOrientations::Portrait ||
		orientation == DisplayOrientations::PortraitFlipped)
	{
		IsActive = Orientation == Orientations::Portrait;
	}
}

void scwin::OrientationStateTrigger::OrientationStateTrigger_OrientationChanged(DisplayInformation^ sender, Object^ args)
{
	UpdateTrigger(sender->CurrentOrientation);
}