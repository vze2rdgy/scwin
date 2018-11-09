#include "pch.h"
#include "DataStateVisibilityTrigger.h"

DependencyProperty^ DataStateVisibilityTrigger::objectStateProperty = nullptr;
DependencyProperty^ DataStateVisibilityTrigger::objectStateTriggerProperty = nullptr;

void scwin::DataStateVisibilityTrigger::Register()
{
	if (objectStateProperty == nullptr)
		objectStateProperty = DependencyProperty::Register("ObjectState",
			DataState::typeid, DataStateVisibilityTrigger::typeid, ref new PropertyMetadata(DataState::Unmodified));
	if (objectStateTriggerProperty == nullptr)
		objectStateTriggerProperty = DependencyProperty::Register("ObjectStateTrigger",
			DataState::typeid, DataStateVisibilityTrigger::typeid, 
			ref new PropertyMetadata(DataState::Unmodified,
				ref new PropertyChangedCallback(&DataStateVisibilityTrigger::OnPropertyChanged)
		));
}

DataStateVisibilityTrigger::DataStateVisibilityTrigger()
{

}

void scwin::DataStateVisibilityTrigger::OnPropertyChanged(DependencyObject^ dp, DependencyPropertyChangedEventArgs ^ eargs)
{
	auto trigger = safe_cast<DataStateVisibilityTrigger^>(dp);
	auto b = trigger->ObjectState == trigger->ObjectStateTrigger;
	trigger->SetActive(b);
#ifdef _DEBUG
	ostringstream ostr;
	switch (trigger->ObjectState)
	{
	case DataState::Added:
		ostr << "Added State ";
			break;
	case DataState::Edited:
		ostr << "Edited State";
		break;
	case DataState::Unmodified:
	default:
		ostr << "Unmodified State ";
		break;
	}
	if (b)
		ostr << " is Activated";
	else
		ostr << " is Deactivated";
	OutputDebugStringA(ostr.str().c_str());
	OutputDebugStringA("\n");
#endif
}
