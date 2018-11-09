#pragma once

namespace scwin
{

	public ref class Date sealed
	{
		__int64 time;
		tm tmstuct;

	public:
		[Windows::Foundation::Metadata::DefaultOverload]
		Date();
		Date(short year, short month, short day, bool isutc);
		Date(short year, short month, short day, short hour, short minute, short second, bool isutc);
		Date(DateTime datetime);
		Date(__int64 unixTime);

		property bool IsValid
		{
			bool get();
		}

		property short Year
		{
			short get() 
			{ 
				return 1900 + tmstuct.tm_year; 
			}
			void set(short val) 
			{ 
				tmstuct.tm_year = val - 1900; 
			}
		}
		property short Month
		{
			short get() 
			{ 
				return tmstuct.tm_mon + 1; 
			}
			void set(short val) 
			{ 
				tmstuct.tm_mon = val - 1; 
			}

		}
		property short Day
		{
			short get() 
			{ 
				return tmstuct.tm_mday; 
			}
			void set(short val) 
			{ 
				tmstuct.tm_mday = val; 
			}

		}
		property short Hour
		{
			short get() 
			{ 
				return tmstuct.tm_hour; 
			}
			void set(short val) 
			{ 
				tmstuct.tm_hour = val; 
			}
		}
		property short Minute
		{
			short get() { return tmstuct.tm_min; }
			void set(short val) { tmstuct.tm_min = val; }

		}
		property short Second
		{
			short get() { return tmstuct.tm_sec; }
			void set(short val) { tmstuct.tm_sec = val; }
		}
		property String^ Timezone;
		property bool IsUtc;
		property bool IsLocal;

		property __int64 UnixTime 
		{
			__int64 get() { return time; }
		}

		static property Date^ Now
		{
			Date^ get();
		}

		static property Date^ UTCNow
		{
			Date^ get();
		}

		property int ShortDateAsNumber
		{
			int get();
		}

		DateTime GetUWPDateTime();

		static Date^ ConvertFromUWPDateTime(DateTime datetime);

		virtual String^ ToString() override;
		String^ ToShortFormatString() ;
	};

}