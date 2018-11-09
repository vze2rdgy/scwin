#pragma once

namespace scwin
{

	public ref class StatusMessage sealed 
	{

	public:
		StatusMessage(String^ message, StatusMessageTypes type);

		property String^ Message;

		property StatusMessageTypes Type;

	};

}