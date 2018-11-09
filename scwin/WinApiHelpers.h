#pragma once

namespace scwin
{
	class WinApiHelpers
	{
	public:

		static String^ GetErrorMessage(DWORD lastErrorCode);

	};

}
