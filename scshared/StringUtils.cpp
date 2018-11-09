#include "pch.h"
#include "StringUtils.h"
#include <vector>
#include <sstream>

using namespace std;
using namespace scshared;

std::vector<std::string> scshared::StringUtils::Split(const std::string & str, char delimiter)
{
	vector<string> strArr;
	if (str.empty())
		return std::move(strArr);

	stringstream ostr;
	size_t len = str.size();
	auto chrs = str.data();
	for (int i = 0; i < len; i++)
	{
		auto c = chrs[i];
		if (c == delimiter)
		{
			string part = ostr.str();
			strArr.emplace_back(part);
			ostr.str("");
		}
		else
		{
			ostr << c;
		}
	}
	if (ostr.str() != "")
		strArr.emplace_back(ostr.str());
	return strArr;
}

std::vector<std::string> scshared::StringUtils::Split(const std::string & str, const std::string & delimiter)
{
	string mutstr = str;
	string token;
	vector<string> parts;
	size_t pos = 0;
	while ((pos = mutstr.find(delimiter)) != string::npos)
	{
		if (mutstr.empty())
			break;
		token = mutstr.substr(0, pos);
		parts.emplace_back(token);
		mutstr.erase(0, pos + delimiter.length());
	}
	if (!mutstr.empty())
		parts.emplace_back(mutstr);
	return std::move(parts);
}

const std::string scshared::StringUtils::Join(const std::vector<std::string>& strarr, char delimiter)
{
	if (!strarr.size())
		return "";
	stringstream ostr;
	for (size_t i = 0; i < strarr.size(); i++)
	{
		ostr << strarr.at(i);
		if (i < strarr.size() - 1)
		{
			ostr << delimiter;
		}
	}
	return std::move(ostr.str());
}
