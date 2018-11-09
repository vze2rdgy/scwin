#include "pch.h"
#include "HashingHelpers.h"
#include "../scshared/CodeGenerator.h"

std::string scwin::HashingHelpers::GetCRC32(const string data)
{
	//auto charArr = data.c_str();
	//boost::crc_basic<8> crc8(0xffff, 0xffff, 0x0, false, false);
	//crc8.process_bytes(charArr, data.length());
	//return std::to_string(crc8.checksum());
	return scshared::CodeGenerator::GetCRCCode32(data);
}
