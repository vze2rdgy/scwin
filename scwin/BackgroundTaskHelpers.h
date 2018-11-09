#pragma once

namespace scwin
{

	ref class BackgroundTaskHelpers sealed
	{
	public:
		static BackgroundTaskRegistration^ RegisterBackgroundTask(
			Platform::String ^ taskEntryPoint,
			Platform::String ^ taskName,
			IBackgroundTrigger ^ trigger,
			IBackgroundCondition ^ condition);

		static void UnregisterBackgroundTask(String^ name);

	};

}