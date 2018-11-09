#pragma once

namespace scwin
{

	class HashingHelpers
	{
	public:
		static std::string GetCRC32(const string data); // note data param not a reference since data will be modified by this fucntion.
	};

}