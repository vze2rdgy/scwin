#include "pch.h"
#include "BackgroundTaskHelpers.h"

using namespace scwin;

BackgroundTaskRegistration^ BackgroundTaskHelpers::RegisterBackgroundTask(
	Platform::String ^ taskEntryPoint,
	Platform::String ^ taskName,
	IBackgroundTrigger ^ trigger,
	IBackgroundCondition ^ condition)
{

	//
	// Check for existing registrations of this background task.
	//

	auto iter = BackgroundTaskRegistration::AllTasks->First();
	auto hascur = iter->HasCurrent;

	while (hascur)
	{
		auto cur = iter->Current->Value;
		if (cur->Name == taskName)
		{
			//
			// The task is registered.
			//
			return (BackgroundTaskRegistration ^)(cur);
		}
		hascur = iter->MoveNext();
	}
	//
	// Register the background task.
	//
	auto builder = ref new BackgroundTaskBuilder();
	builder->Name = taskName;
	builder->TaskEntryPoint = taskEntryPoint;
	builder->SetTrigger(trigger);

	if (condition != nullptr) {

		builder->AddCondition(condition);
	}
	BackgroundTaskRegistration ^ task = builder->Register();
	return task;
}

void scwin::BackgroundTaskHelpers::UnregisterBackgroundTask(String^ name)
{
	//
	// Loop through all background tasks and unregister any that have a name that matches
	// the name passed into this function.
	//
	auto iter = BackgroundTaskRegistration::AllTasks->First();
	auto hascur = iter->HasCurrent;
	while (hascur)
	{
		auto cur = iter->Current->Value;

		if (cur->Name == name)
		{
			cur->Unregister(true);
		}
		hascur = iter->MoveNext();
	}
}
