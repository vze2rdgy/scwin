#include "pch.h"
#include "StringHelpers.h"
#include <regex>
#include <sstream>
#include <boost\algorithm\string.hpp>

using namespace std;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;

//StringHelpers::StringHelpers()
//{
//}

String ^ scwin::utils::StringHelpers::Trim(String ^ s)
{
	const char16* first = s->Begin();
	const char16* last = s->End();
	while (first != last && iswspace(*first))
	{
		++first;
	}

	while (first != last && iswspace(last[-1]))
	{
		--last;
	}

	return ref new String(first, (int)(last - first));
}

bool scwin::utils::StringHelpers::IsNullOrEmpty(String ^ s)
{
	return s == nullptr || s->IsEmpty();
}

bool scwin::utils::StringHelpers::StartsWith(String ^ s, char16 chr)
{
	const char16* first = s->Begin();
	if (first && (*first) == chr)
		return true;
	return false;
}

String ^ scwin::utils::StringHelpers::Substring(String ^ s, unsigned int start, unsigned int end)
{
	if (IsNullOrEmpty(s))
	{
		return L""; 
	}
	if (end == MAXSIZE_T)
	{
		end = s->Length() - 1;
	}
	if (s->Length() > 0U && start >= 0U && end < s->Length() && end >= start)
	{
		auto data = s->Data();
		String^ str = ref new String();
		for (UINT u = start; u <= end; u++)
		{
			str += data[u];
		}
		return str;
	}
	return L"";
}


String ^ scwin::utils::StringHelpers::Replace(String ^ s, String ^ val, String ^ replaceWith)
{
	if (IsNullOrEmpty(s) || IsNullOrEmpty(val))
	{
		return s;
	}
	std::wstring oldVal = val->Data();
	std::wstring newVal = replaceWith->Data();
	std::wstring str = s->Data();
	std::wstring newStr;

	boost::replace_all(str, oldVal, newVal);

	return ref new String(str.c_str());
}

std::string scwin::utils::StringHelpers::SqlSafeConvert(String ^ s)
{
	if (StringHelpers::IsNullOrEmpty(s))
		return "";

	const auto replace = L"'";
	const auto replacewith = L"''";
	std::wstring str = s->Data();
	boost::replace_all(str, replace, replacewith);
	return Convert(str);
}

std::wstring scwin::utils::StringHelpers::Convert(const std::string & str)
{
	size_t reqSize = str.size() + sizeof(wchar_t);
	wchar_t* dest = new wchar_t[reqSize];
	auto ret = _mbstowcs_s_l(&reqSize, dest, reqSize, str.c_str(), str.size(), 0);
	if (ret != 0)
	{
		char err[4096];
		strerror_s(err, ret);
		LogException(err);
		throw std::runtime_error(err);
	}
	auto& sret = std::wstring(dest);
	delete[] dest;
	return sret;
}

std::string scwin::utils::StringHelpers::Convert(const std::wstring & str)
{
	size_t reqSize = str.size() + sizeof(char);
	char* dest = new char[reqSize]; //[0x800];
	auto ret = _wcstombs_s_l(&reqSize, dest, reqSize, str.c_str(), str.size(), 0);
	if (ret != 0)
	{
		char err[4096];
		strerror_s(err, ret);
		LogException(err);
		throw std::runtime_error(err);
	}
	auto& sret = std::string(dest);
	delete[] dest;
	return sret;
}

//std::string scwin::utils::StringHelpers::Convert(const char16 * str)
//{
//	size_t reqSize = wcslen(str);
//	char* dest = new char[reqSize]; //[0x800];
//	auto ret = _wcstombs_s_l(&reqSize, dest, reqSize, str, reqSize, 0);
//	if (ret != 0)
//	{
//		char err[4096];
//		strerror_s(err, ret);
//		LogException(err);
//		throw std::runtime_error(err);
//	}
//	auto& sret = std::string(dest);
//	delete[] dest;
//	return sret;
//}

std::string scwin::utils::StringHelpers::Convert(String ^ refstr) 
{
	if (!refstr)
		return std::move((string)"");
	return Convert(refstr->Data());
}

String ^ scwin::utils::StringHelpers::Convert(char * str)
{
	if (!str)
		return nullptr;

	std::wstring wstr = Convert((std::string)str);
	return ref new String(wstr.c_str());
}

String ^ scwin::utils::StringHelpers::ConvertToString(const char * str)
{
	if (!str)
		return nullptr;

	std::wstring wstr = Convert((std::string)str);
	return ref new String(wstr.c_str());
}

bool scwin::utils::StringHelpers::Match(String ^ text, String ^ pattern, bool casesensitive)
{
	if (IsNullOrEmpty(pattern))
	{
		return true;
	}
	const auto& str = text->Data();
	const auto& pat = pattern->Data();
	if (casesensitive)
	{
		return !!std::wcsstr(str, pat);
	}
	else
	{
		wchar_t* lupper = const_cast<wchar_t*>(str);
		wchar_t* rupper = const_cast<wchar_t*>(pat);
		_wcsupr_s(lupper, text->Length() + 1);
		_wcsupr_s(rupper, pattern->Length() + 1);
		return !!std::wcsstr(lupper, rupper);
	}
}

int64 scwin::utils::StringHelpers::IndexOf(String ^ text, String ^ what)
{
	if (IsNullOrEmpty(text) || IsNullOrEmpty(what))
	{
		return -1;
	}
	auto span = wcscspn(text->Data(), what->Data());
	if (span == text->Length())
		return -1;
	else
		return span;
}

bool scwin::utils::StringHelpers::IsValidEmail(String ^ email)
{
	if (IsNullOrEmpty(email))
		return false;
	auto utf8email = Convert(email->Data());
	//auto pattern = R"(^(?(\")(\".+?(?<!\\)\"@) | (([0 - 9a - z]((\.(? !\.)) | [-!#\$%&'\*\+/=\?\^`\{\}\|~\w])*)(?<=[0-9a-z])@))(?(\[)(\[(\d{1,3}\.){3}\d{1,3}\])|(([0-9a-z][-\w]*[0-9a-z]*\.)+[a-z0-9][\-a-z0-9]{0,22}[a-z0-9]))$)";
	auto pattern = R"(^\S+@\S+$)";
	std::regex rg(pattern);
	return std::regex_match(utf8email, rg);
}

bool scwin::utils::StringHelpers::IsValidPassword(String ^ pwd)
{
	//TODO improve this later
	return !IsNullOrEmpty(pwd) && pwd->Length()>6;
}

String ^ scwin::utils::StringHelpers::ByteToHex(byte b)
{
	int const arraysize = 3;
	char16 hexValue[arraysize] = { 0 };
	swprintf_s(hexValue, arraysize, L"%02x", (int)b);
	return ref new String(hexValue);
}

byte scwin::utils::StringHelpers::HexToByte(String ^ hex)
{
	int b;
	std::istringstream istr(STRCONV(hex));
	istr >> std::hex >> b;
	return (byte)b;
}

Array<byte>^ scwin::utils::StringHelpers::HexDataToBytes(String ^ lengthyhex)
{
	Array<byte>^ bytes;
	IBuffer^ buffer = CryptographicBuffer::DecodeFromHexString(lengthyhex); // will throw exception if lengthyhex is not right.
	CryptographicBuffer::CopyToByteArray(buffer, &bytes);
	return bytes;
}

String ^ scwin::utils::StringHelpers::GetMemoryAddress(int databyte)
{
	int const arraysize = 9;
	char16 address[arraysize] = { 0 };
	swprintf_s(address, arraysize, L"%08x", databyte);
	return ref new String(address);
}

IVector<String^>^ scwin::utils::StringHelpers::Split(String ^ str, char16 delimiter)
{
	auto strArr = ref new Vector<String^>();
	if (IsNullOrEmpty(str))
		return strArr;
	wstringstream ostr;
	int len = str->Length();
	auto chrs = str->Data();
	for (int i = 0; i < len; i++)
	{
		auto c = chrs[i];
		if (c == delimiter)
		{
			String^ part = ref new String(ostr.str().c_str());
			strArr->Append(part);
			ostr.str(L"");
		}
		else
		{
			ostr << c;
		}
	}
	if (ostr.str() != L"")
		strArr->Append(ref new String(ostr.str().c_str()));
	return strArr;
}

std::vector<string> scwin::utils::StringHelpers::Split(std::string str, std::string delimiter)
{
	string token;
	vector<string> parts;
	UINT pos = 0;
	while ((pos = str.find(delimiter)) != string::npos)
	{
		if (str.empty())
			break;
		token = str.substr(0, pos);
		parts.emplace_back(token);
		str.erase(0, pos + delimiter.length());
	}
	if (!str.empty())
		parts.emplace_back(str);
	return std::move(parts);
}

String ^ scwin::utils::StringHelpers::Join(IVector<String^>^ strarr, char16 delimiter)
{
	if (!strarr || !strarr->Size)
		return L"";
	wstringstream ostr;
	for(int i = 0; i < strarr->Size; i++)
	{
		ostr << strarr->GetAt(i)->Data();
		if (i < strarr->Size - 1)
		{
			ostr << delimiter;
		}
	}
	return ref new String(ostr.str().c_str());
}

bool scwin::utils::StringHelpers::RemoveAtEnd(String ^ str, String ^ toberemoved, String^* output)
{
	if (StringHelpers::IsNullOrEmpty(str) || StringHelpers::IsNullOrEmpty(toberemoved))
		return false;

	if (str->Length() < toberemoved->Length())
		return false;

	const char16* strarr = str->Data();
	const char16* remsarr = toberemoved->Data();

	bool hasreplaced = false;
	auto len = str->Length();

	int startindex = len - toberemoved->Length();
	wstring whatlastthree(strarr + startindex, toberemoved->Length());
	if (wcscmp(whatlastthree.c_str(), remsarr) == 0)
	{
		wstring newstr(strarr, startindex);
		*output = ref new String(newstr.c_str());
		hasreplaced = true;
	}
	else
	{
		*output = str;
	}
	return hasreplaced;
}

