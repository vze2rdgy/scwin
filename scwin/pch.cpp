//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"
#include <sstream>
#include "FileLogging.h"

//critical_section gLock;
std::recursive_mutex gLock;

/// <summary> ALL application messages </summary>
// note. Messages variable is allocated in heap, but not deleted because 
// this memory is used by app during its lifetime and automatically removed when
// application closes.
char16** const Messages = new char16*[29]
{
	L"The operation was successfull.",
	L"The operation is successful, but with conditions",
	L"Operation cannot proceed due to internet unavailability.",
	L"Session cannot be verified. Please sign in.",
	L"Failed to complete the signup process for unknown reason. Please retry.",
	L"Failed to complete the signin process for unknown reason. Please retry.",
	L"Failed to clear database files.",
	L"No user profile is selected. Please cancel and retry.",
	L"Text entered is not a valid email address. Please correct.",
	L"Password doesn't conform to application standards (must be atleast 6 characters).",
	L"User's full name is not provided.",
	L"Failed to save information.",
	L"Passwords do not match.",
	L"Replication Error: Data from the remote source was not saved correctly in the cache database.",
	L"Replication Error: Data to the remote source was not correctly saved.",
	L"Data query exception. Cannot continue.",
	L"The product attribute value's data type doesn't match with DataType property.",
	L"Not all required fields are entered.",
	L"Invalid argument provided. Verify if the argument passed is null.",
	L"Argument provided is outside the allowed range.",
	L"DB Transaction failed to initialized.",
	L"DB Transaction failed during commit.",
	L"DB Update Failed.",
	L"Not implemented.",
	L"Duplicate document id. A document with this id already exists.",
	L"The information is used and cannot be removed.",
	L"Duplicated information",
	L"This provided object has one or more required properties set to an invalid value.",
	L"Remote method call failed to execute."
};


inline void ThrowException(MessageIds id)
{
	auto msg = ref new String(Messages[(int)id]);
	throw Exception::CreateException (MAKE_HRESULT(1, 0, (int)id), msg);
}

inline Exception^ CreateException(MessageIds id)
{
	auto msg = ref new String(Messages[(int)id]);
	return Exception::CreateException(MAKE_HRESULT(1, 0, (int)id), msg);
}

inline void LogException(LPCSTR message)
{
	//TODO: before staging the app, a real logging 
	// method will be used (such as EventLog).
#ifdef DEBUG
	OutputDebugStringA(message);
	FileLoggingSessionScenario::Instance->LogCritical(StringHelpers::ConvertToString(message));
#else
	FileLoggingSessionScenario::Instance->LogCritical(StringHelpers::ConvertToString(message));
#endif
}

inline void LogException(MessageIds id)
{
	ostringstream ostr;
	ostr << "Exception logged: MessageId: " << (int)id << ", Message: " << STRCONV(Messages[(int)id]);
	auto cstr = ostr.str().c_str();
#ifdef DEBUG
	OutputDebugStringA(cstr);
	FileLoggingSessionScenario::Instance->LogCritical(StringHelpers::ConvertToString(cstr));
#else
	FileLoggingSessionScenario::Instance->LogCritical(StringHelpers::ConvertToString(cstr));
#endif
}

inline MessageIds LogRetException(MessageIds id)
{
	LogException(id);
	return id;
}

inline MessageIds LogRetException(MessageIds id, LPCSTR message)
{
	LogException(message);
	return id;
}

inline char16 * GetMessage(MessageIds id)
{
	return Messages[(int)id];
}

std::recursive_mutex GlobalMutex;