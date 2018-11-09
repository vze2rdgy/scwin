#pragma once

#include <string>
#include <vector>


namespace scshared
{
	class StringUtils
	{
	public:
		/// <summary> Split a string into an array using the delimiter seperating them </summary>
		static std::vector<std::string> Split(const std::string& str, char delimiter);

		static std::vector<std::string> Split(const std::string& str, const std::string& delimiter);

		/// <summary> Join an array of strings into a single string by seperating them with the delimiter </summary>
		static const std::string Join(const std::vector<std::string>& strarr, char delimiter);
	};

}