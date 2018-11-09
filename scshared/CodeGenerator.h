#pragma once

#include <string>

namespace scshared
{

	class CodeGenerator
	{
	public:
		static std::string GetNextOrderNumber(const std::string& lastEncodedData, const std::string& userid, std::string * const newlyEncodedData);
		static std::string GetCRCCode32(const std::string& stringval);
	};

}