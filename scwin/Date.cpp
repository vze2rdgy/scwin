#include "pch.h"
#include "Date.h"
#include <time.h>
#include "WinApiHelpers.h"

using namespace Windows::Globalization;

Date::Date()
{
}

scwin::Date::Date(short year, short month, short day, bool isutc)
{
	tmstuct.tm_year = year - 1900;
	tmstuct.tm_mon = month - 1;
	tmstuct.tm_mday = day ;
	IsUtc = isutc;
	IsLocal = !isutc;
	time = mktime(&tmstuct);
}

scwin::Date::Date(short year, short month, short day, short hour, short minute, short second, bool isutc)
{
	tmstuct.tm_year = year - 1900;
	tmstuct.tm_mon = month - 1;
	tmstuct.tm_mday = day ;
	tmstuct.tm_hour = hour;
	tmstuct.tm_min = minute;
	tmstuct.tm_sec = second;
	IsUtc = isutc;
	IsLocal = !isutc;
	time = mktime(&tmstuct);
}

scwin::Date::Date(DateTime datetime)
{
	auto cal = ref new Calendar();
	cal->SetDateTime(datetime);
	Day = cal->Day ;
	Month = cal->Month ;
	Year = cal->Year ;
	if (cal->Period == 2)
		Hour = 12 + cal->Hour;
	else
		Hour = cal->Hour - 1;
	Minute = cal->Minute;
	Second = cal->Second;
	IsLocal = true;
	IsUtc = false;
	time = mktime(&tmstuct);
}

scwin::Date::Date(__int64 unixTime) : time(unixTime)
{
	if (unixTime > 0)
	{
		IsLocal = true;
		IsUtc = false;
		ZeroMemory(&tmstuct, sizeof(tmstuct));
		localtime_s(&tmstuct, &unixTime);
	}
}

bool Date::IsValid::get()
{
	return time > 0;
}

DateTime scwin::Date::GetUWPDateTime()
{
	SYSTEMTIME systime;
	ZeroMemory(&systime, sizeof(systime));
	auto tm = this->tmstuct;
	if (IsLocal)
	{
		gmtime_s(&tm, &this->time);
	}
	systime.wDay = tm.tm_mday;
	systime.wMonth = tm.tm_mon + 1;
	systime.wYear = 1900 + tm.tm_year;
	systime.wHour = tm.tm_hour;
	systime.wMinute = tm.tm_min;
	systime.wSecond = tm.tm_sec;

	FILETIME fileTime;
	ZeroMemory(&fileTime, sizeof(fileTime));
	if (SystemTimeToFileTime(&systime, &fileTime))
	{
		ULARGE_INTEGER uli;
		uli.HighPart = fileTime.dwHighDateTime;
		uli.LowPart = fileTime.dwLowDateTime;
		DateTime dtime;
		dtime.UniversalTime = uli.QuadPart;
		return dtime;
	}

	// should be an error thrown ?
	return DateTime();
}

Date ^ scwin::Date::ConvertFromUWPDateTime(DateTime datetime)
{
	return ref new Date(datetime);
}

String ^ scwin::Date::ToShortFormatString()
{
	if (time > 0)
	{
		char buff[256];
		asctime_s<256>(buff, &tmstuct);
		strftime(buff, 256, "%F", &tmstuct);
		return STRCONV(buff);
	}
	else
		return "";
}

String ^ scwin::Date::ToString()
{
	if (time > 0)
	{
		char buff[256];
		asctime_s<256>(buff, &tmstuct);
		return STRCONV(buff);
	}
	else
		return "";
}

Date^ Date::Now::get()
{
	time_t timeNow = std::time(nullptr);
	// get lcoal time
	Date^ dt = ref new Date();
	localtime_s(&dt->tmstuct, &timeNow);
	dt->IsLocal = true;
	dt->IsUtc = false;
	dt->time = timeNow;
	return dt;
}

Date^ Date::UTCNow::get()
{
	time_t timeNow = std::time(nullptr);
	Date^ dt = ref new Date();
	gmtime_s(&dt->tmstuct, &timeNow);
	dt->IsLocal = false;
	dt->IsUtc = true;
	dt->time = timeNow;
	return dt;
}

int Date::ShortDateAsNumber::get()
{
	return (int)(Year << 3 | Month << 2 | Day);
}