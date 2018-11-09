#include "pch.h"
#include "DateTimeHelpers.h"

using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;

DateTime scwin::DateTimeHelpers::GetLocalCurrent()
{
	auto calendar = ref new Calendar();
	return calendar->GetDateTime();
}

String ^ scwin::DateTimeHelpers::GetDateTimeForTimezoneAsString(DateTime dt, String ^ format, String ^ timezone)
{
	auto dtformatter = ref new DateTimeFormatter(format);
	if (timezone == nullptr)
		return dtformatter->Format(dt);
	else
		return dtformatter->Format(dt, timezone);
}

