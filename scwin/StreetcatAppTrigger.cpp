#include "pch.h"
#include "StreetcatAppTrigger.h"

using namespace scwin;

DependencyProperty^ StreetcatAppTrigger::isAuthorizedProperty = nullptr;

void scwin::StreetcatAppTrigger::UpdateTrigger()
{
	// may not need this since IsAuthorized property is a notifying type of property.
}

void scwin::StreetcatAppTrigger::RegisterDependencyProperties()
{
	if (isAuthorizedProperty == nullptr)
	{
		auto meta = PropertyMetadata::Create(
			ref new CreateDefaultValueCallback([]
			{
				auto obj = ref new StreetcatAppTrigger();
				obj->IsAuthorized = false;
				return obj;
			}),
			ref new PropertyChangedCallback(OnIsAuthorizedPropertyChanged)
		);
		isAuthorizedProperty = DependencyProperty::Register(
			"IsAuthorized",
			Platform::Boolean::typeid,
			StreetcatAppTrigger::typeid,
			meta);
	}
}

DependencyProperty^ StreetcatAppTrigger::IsAuthorizedProperty::get()
{
	return isAuthorizedProperty;
}

void scwin::StreetcatAppTrigger::OnIsAuthorizedPropertyChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ e)
{
	auto obj = safe_cast<StreetcatAppTrigger^>(d);
	if (obj)
	{
		obj->UpdateTrigger();
	}
}

StreetcatAppTrigger::StreetcatAppTrigger()
{

}

bool StreetcatAppTrigger::IsAuthorized::get()
{
	return (bool)GetValue(IsAuthorizedProperty);
}

void StreetcatAppTrigger::IsAuthorized::set(bool val)
{
	SetValue(IsAuthorizedProperty, val);
}
