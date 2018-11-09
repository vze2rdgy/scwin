#include "pch.h"
#include "CodeGenerator.h"
#include <vector>
#include "StringUtils.h"
#include <boost\crc.hpp>
#include <iostream>

using namespace std;
using namespace scshared;

std::string CodeGenerator::GetNextOrderNumber(const std::string& lastEncodedData, const std::string& userid, std::string * const newlyEncodedData)
{
	string numbuckets = lastEncodedData;
	// There are five buckets with each bucket value between 0 to 25 representing an alpha character.
	// alternate numbers are counted in reverse.
	const char* alphaArray[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
	size_t bucketCount = 5;
	if (numbuckets.empty())
		numbuckets = "0,0,0,0,-1";
	vector<string> buckets = StringUtils::Split(numbuckets, ",");

	// if all bucket memers are already 25, then insert into bucket item at the top
	// and set all to zero
	bool isoverflowing = false;

BUCKETSTEPS:

	for (size_t i = buckets.size() - 1; i >= 0; i--)
	{
		auto val = buckets[i];
		auto ival = std::strtol(val.c_str(), nullptr, 10);
		if (ival == 25)
		{
			if (i == 0)
			{
				isoverflowing = true;
				break;
			}
			else
			{
				// increment prior bucket and set this bucket to 0
				buckets[i - 1] = std::to_string(std::strtol(buckets[i - 1].c_str(), nullptr, 10) + 1);
				buckets[i] = "0";
				break;
			}
		}
		else
		{
			// every altenate value char is revearse alpha
			// ie. first is 0-25, next is 25-0, and so on.
			// we are only bothered with one bucket. Above
			// continue statement will ensure that
			// bucket member is shifted left when current
			// bucket member has "overflown"
			buckets[i] = std::to_string(++ival);
			break;
		}
	}

	if (isoverflowing)
	{
		buckets.insert(buckets.cbegin(), "0");
		for (size_t i = 1; i < buckets.size() - 1; i++)
			buckets[i] = "0";
		buckets[buckets.size() - 1] = "-1";

		bucketCount = buckets.size();

		goto BUCKETSTEPS;
	}

	string output;
	string joined;
	for (int i = 0; i < buckets.size(); i++)
	{
		auto val = buckets[i];
		auto ival = std::strtol(val.c_str(), nullptr, 10);
		if (0 == (i % 2))
		{
			output += alphaArray[ival];
		}
		else
		{
			output += alphaArray[25 - ival];
		}
		joined += val;
		if (i < buckets.size() - 1)
			joined += ",";
	}

	output += "-";
	output += GetCRCCode32(userid.c_str());
	if (newlyEncodedData != nullptr)
		*newlyEncodedData = joined;
	return output;
}

std::string scshared::CodeGenerator::GetCRCCode32(const string& data)
{
	auto charArr = data.c_str();
	boost::crc_basic<16> crc32(0xffff, 0xffff, 0x0, false, false);
	crc32.process_bytes(charArr, data.length());
	return std::to_string(crc32.checksum());
}
