#pragma once

namespace scwin
{
	// NOTE: User Windows::Globalization::Calendar class for add months, years, etc 

	class DateTimeHelpers
	{
	public:
		static DateTime GetLocalCurrent();
		static String^ GetDateTimeForTimezoneAsString(DateTime dt, String^ format = "longdate longtime", String^ timezone = nullptr);
		
		//static DateTime GetUTCCurrent();
		//static DateTime ConvertLocalToUTC(DateTime local);
		//static DateTime ConvertUTCToLocal(DateTime utc);
	};

}



/* examples */
// SVN: streetcat\samples\UWPSamples\Samples\DateTimeFormatting\cpp

/*

// This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
// in order to display dates and times using basic formatters.

//Get the current application context language
String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

// Formatters for dates and times
Array<DateTimeFormatter^> ^dateFormatters = {
ref new DateTimeFormatter("shortdate"),
ref new DateTimeFormatter("longdate"),
ref new DateTimeFormatter("shorttime"),
ref new DateTimeFormatter("longtime")
};

// Obtain the date that will be formatted.
Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();

// Keep the results here
String^ results = "";
results = results + "Current application context language: " + currentLanguage + "\n\n";

// Generate the results.
for (auto formatter : dateFormatters)
{
// Perform the actual formatting.
results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
}

// Display the results.
OutputTextBlock->Text = results;

*/

/*
// This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
// to format a date/time via a string template.  Note that the order specifed in the string pattern does
// not determine the order of the parts of the formatted string.  The current application language and region value will
// determine the pattern of the date returned based on the specified parts.

//Get the current application context language
String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

// Formatters for dates.
Array<DateTimeFormatter^>^ dateFormatters = {
ref new DateTimeFormatter("month day"),
ref new DateTimeFormatter("month year"),
ref new DateTimeFormatter("month day year"),
ref new DateTimeFormatter("month day dayofweek year"),
ref new DateTimeFormatter("dayofweek.abbreviated"),
ref new DateTimeFormatter("month.abbreviated"),
ref new DateTimeFormatter("year.abbreviated")
};

// Formatters for time.
Array<DateTimeFormatter^>^ timeFormatters = {
ref new DateTimeFormatter("hour minute"),
ref new DateTimeFormatter("hour minute second"),
ref new DateTimeFormatter("hour")
};

// Formatters for timezone.
Array<DateTimeFormatter^>^ timeZoneFormatters = {
ref new DateTimeFormatter("timezone"),
ref new DateTimeFormatter("timezone.full"),
ref new DateTimeFormatter("timezone.abbreviated")
};

// Formatters for combinations.
Array<DateTimeFormatter^>^ combinationFormatters = {
ref new DateTimeFormatter("hour minute second timezone.full"),
ref new DateTimeFormatter("day month year hour minute timezone"),
ref new DateTimeFormatter("dayofweek day month year hour minute second"),
ref new DateTimeFormatter("dayofweek.abbreviated day month hour minute"),
ref new DateTimeFormatter("dayofweek day month year hour minute second timezone.abbreviated")
};

// Obtain the date that will be formatted.
Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();

// Keep the results here
String^ results = "";
results = results + "Current application context language: " + currentLanguage + "\n\n";
results = results + "Dates:\n";

// Generate the results.
for (auto formatter : dateFormatters)
{
// Perform the actual formatting.
results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
}

results = results + "\n";
results = results + "Times:\n";

// Generate the results.
for (auto formatter : timeFormatters)
{
// Perform the actual formatting.
results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
}

results = results + "\n";
results = results + "Time zones:\n";

// Generate the results.
for (auto formatter : timeZoneFormatters)
{
// Perform the actual formatting.
results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
}

results = results + "\n";
results = results + "Date and time combinations:\n";

// Generate the results.
for (auto formatter : combinationFormatters)
{
// Perform the actual formatting.
results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
}

// Display the results.
OutputTextBlock->Text = results;

*/

/*
// This scenario illustrates time zone support in DateTimeFormatter class

// Displayed time zones (other than local time zone)
auto timeZones = ref new Vector<String^>({ "UTC", "America/New_York", "Asia/Kolkata" });

// Keep the results here
String^ results = "";

// Create formatter object using longdate and longtime template
DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("longdate longtime");

// Create date/time to format and display.
Windows::Globalization::Calendar^ calendar = ref new Windows::Globalization::Calendar();
Windows::Foundation::DateTime dateToFormat = calendar->GetDateTime();

// Show current time.
results += "Current date and time\n" +
"In Local time zone: " + dateFormatter->Format(dateToFormat) + "\n";
for (auto timeZone : timeZones)
{
results += "In " + timeZone + " time zone: " + dateFormatter->Format(dateToFormat, timeZone) + "\n";
}
results += "\n";

// Show a time on 14th day of second month of next year.
// Note the effect of daylight saving on the results.
calendar->AddYears(1); calendar->Month = 2; calendar->Day = 14;
dateToFormat = calendar->GetDateTime();
results += "Same time on 14th day of second month of next year\n" +
"In Local time zone: " + dateFormatter->Format(dateToFormat) + "\n";
for (auto timeZone : timeZones)
{
results += "In " + timeZone + " time zone: " + dateFormatter->Format(dateToFormat, timeZone) + "\n";
}
results += "\n";

// Show a time on 14th day of tenth month of next year.
// Note the effect of daylight saving on the results.
calendar->AddMonths(8);
dateToFormat = calendar->GetDateTime();
results += "Same time on 14th day of tenth month of next year\n" +
"In Local time zone: " + dateFormatter->Format(dateToFormat) + "\n";
for (auto timeZone : timeZones)
{
results += "In " + timeZone + " time zone: " + dateFormatter->Format(dateToFormat, timeZone) + "\n";
}
results += "\n";

// Display the results.
OutputTextBlock->Text = results;

*/