#pragma once

namespace scwin
{
	namespace tasks
	{
		ref class ServerStatusCheckBTask sealed :
			public IBackgroundTask
		{
		public:
			ServerStatusCheckBTask();
			virtual ~ServerStatusCheckBTask();

			virtual void Run(IBackgroundTaskInstance^ taskInstance);
		};
	}
}