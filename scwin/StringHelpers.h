#pragma once

using namespace Platform;

namespace scwin
{
	namespace utils
	{
		static class StringHelpers sealed
		{
		//protected private:
		//	StringHelpers();

		public:
			static String^ Trim(String^ s);
			static bool IsNullOrEmpty(String^ s);
			static bool StartsWith(String^s, char16 chr);
			static String^ Substring(String^ s, unsigned int start, unsigned int end = MAXSIZE_T);
			static String^ Replace(String^ s, String^ val, String^ replaceWith);
			static std::string SqlSafeConvert(String^ s);
			static std::wstring Convert(const std::string& str);
			static std::string Convert(const std::wstring& str);
			//static std::string Convert(const char16* str);
			static std::string Convert(String^ refstr); // throws exception if refstr is null

			static String^ Convert(char* str);
			static String^ ConvertToString(const char* str);

			static bool Match(String^ text, String^ pattern, bool casesensitive = false);
			static int64 IndexOf(String^ text, String^ what);


			// validations
			static bool IsValidEmail(String^ email);
			static bool IsValidPassword(String^ pwd);

			/// <summary>Obtain a hex string that represents a byte. </summary>
			static String^ ByteToHex(byte b);

			static byte HexToByte(String^ hex);

			/// <summary>Windows provides some nice utilities like 
			/// https://docs.microsoft.com/en-us/windows/uwp/security/copy-to-and-from-byte-arrays,
			/// https://docs.microsoft.com/en-us/windows/uwp/audio-video-camera/imaging
			/// https://docs.microsoft.com/en-us/windows/uwp/security/convert-between-strings-and-binary-data
			/// </summary>
			static Array<byte>^ HexDataToBytes(String^ lengthyhex);
			
			/// <summary>Returns a string that represents an 8 char memory address.</summary>
			static String^ GetMemoryAddress(int databyte);

			/// <summary> Split a string into an array using the delimiter seperating them </summary>
			static IVector<String^>^ Split(String^ str, char16 delimiter);

			static std::vector<string> Split(std::string str, std::string delimiter);

			/// <summary> Join an array of strings into a single string by seperating them with the delimiter </summary>
			static String^ Join(IVector<String^>^ strarr, char16 delimiter);

			template<typename TList>
			static String^ JoinContainer(const TList& strarr, char16 delimiter);

			static bool RemoveAtEnd(String^ str, String^ toberemoved, String^* output);
		};

		template<typename TList>
		inline String ^ StringHelpers::JoinContainer(const TList & strarr, char16 delimiter)
		{
			if (!strarr.size())
				return L"";

			auto sizeminusone = strarr.size() - 1;
			wstringstream ostr;
			size_t idx = 0;
			for (auto itr = strarr.begin(); itr != strarr.end(); ++itr)
			{
				ostr << (*itr)->Data();
				if (idx < sizeminusone)
				{
					ostr << delimiter;
				}
				idx++;
			}
			return ref new String(ostr.str().c_str());
		}
	}

#define ISEMPTYSTR(str) scwin::utils::StringHelpers::IsNullOrEmpty(str)
}
