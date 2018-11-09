#include "pch.h"
#include "DeviceFamilyStateTrigger.h"

DependencyProperty^ DeviceFamilyStateTrigger::deviceFamilyProperty = nullptr;
String^ DeviceFamilyStateTrigger::deviceFamily = nullptr;

void scwin::DeviceFamilyStateTrigger::Register()
{
	if (deviceFamilyProperty == nullptr)
	{
		deviceFamilyProperty =
			DependencyProperty::Register
			(
				"DeviceFamily",
				DeviceFamily::typeid,
				DeviceFamilyStateTrigger::typeid,
				ref new PropertyMetadata
				(
					DeviceFamily::Unknown,
					ref new PropertyChangedCallback(&DeviceFamilyStateTrigger::OnDeviceTypePropertyChanged)
				)
			);
	}

	//auto qualifiers = Windows::ApplicationModel::Resources::Core::ResourceContext::GetForCurrentView()->QualifierValues;
	//if (qualifiers->HasKey("DeviceFamily"))
	//	deviceFamily =  qualifiers->Lookup("DeviceFamily");
	//else
	deviceFamily = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily;
}

DependencyProperty^ DeviceFamilyStateTrigger::DevicefamilyProperty::get()
{
	return deviceFamilyProperty;
}

DeviceFamilyStateTrigger::DeviceFamilyStateTrigger()
{
}

DeviceFamily DeviceFamilyStateTrigger::Devicefamily::get()
{
	return (DeviceFamily)GetValue(deviceFamilyProperty);
}

void DeviceFamilyStateTrigger::Devicefamily::set(DeviceFamily val)
{
	SetValue(deviceFamilyProperty, val);
}

bool DeviceFamilyStateTrigger::IsActive::get()
{
	return this->isActive;
}

void DeviceFamilyStateTrigger::IsActive::set(bool val)
{
	isActive = val;
	SetActive(val);
	IsActiveChanged(this, val);
}

void scwin::DeviceFamilyStateTrigger::OnDeviceTypePropertyChanged(DependencyObject ^ d, DependencyPropertyChangedEventArgs ^ e)
{
	auto trigger = (DeviceFamilyStateTrigger^)d;
	auto val = (DeviceFamily)e->NewValue;

	/*
	if (deviceFamily == "Windows.Mobile")
	obj.IsActive = (val == DeviceFamily.Mobile);
	else if (deviceFamily == "Windows.Desktop")
	obj.IsActive = (val == DeviceFamily.Desktop);
	else if (deviceFamily == "Windows.Team")
	obj.IsActive = (val == DeviceFamily.Team);
	else if (deviceFamily == "Windows.IoT")
	obj.IsActive = (val == DeviceFamily.IoT);
	else if (deviceFamily == "Windows.Holographic")
	obj.IsActive = (val == DeviceFamily.Holographic);
	else if (deviceFamily == "Windows.Xbox")
	*/
	if (deviceFamily == "Windows.Mobile")
	{
		trigger->IsActive = val == DeviceFamily::Phone;
	}
	else if (deviceFamily == "Windows.Desktop")
	{
		trigger->IsActive = val == DeviceFamily::Desktop ;
	}
	else if (deviceFamily == "Windows.Xbox")
	{
		trigger->IsActive = val == DeviceFamily::XBox;
	}
	else
	{
		trigger->IsActive = val == DeviceFamily::Desktop;
	}
}

