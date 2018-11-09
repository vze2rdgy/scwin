#include "pch.h"
#include "SysInfoHelpers.h"

using namespace scwin::utils;

bool scwin::utils::SysInfoHelpers::IsTenFootExperience()
{
	return IsXbox();
}

bool scwin::utils::SysInfoHelpers::IsXbox()
{
	using namespace Windows::System::Profile;
	return AnalyticsInfo::VersionInfo->DeviceFamily == "Windows.Xbox";
}

bool scwin::utils::SysInfoHelpers::IsInternetAvailable()
{
	//https://docs.microsoft.com/en-us/uwp/api/Windows.Networking.Connectivity

	using namespace Windows::Networking::Connectivity;
	ConnectionProfile^ inetProf =
		NetworkInformation::GetInternetConnectionProfile();
	if (inetProf != nullptr &&
		inetProf->GetNetworkConnectivityLevel() == NetworkConnectivityLevel::InternetAccess)
		return true;
	return false;
}

Platform::Guid scwin::utils::SysInfoHelpers::NewGUID()
{
	HRESULT hr;
	GUID g;
	if (SUCCEEDED(hr = CoCreateGuid(&g)))
	{
		return Platform::Guid(g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
	}
	throw ref new COMException(hr);
}

String ^ scwin::utils::SysInfoHelpers::NewCouchDbId()
{
	HRESULT hr;
	GUID g;
	if (FAILED(hr = CoCreateGuid(&g)))
	{
		throw ref new COMException(hr);
	}
	OLECHAR buff[40];
	if (!StringFromGUID2(g, buff, 40))
	{
		throw Exception::CreateException(E_FAIL, "Failed to convert GUID to String");
	}
	auto len = wcslen(buff);
	char16 strGuid[33];
	size_t index = 0;
	for (size_t c = 0; c < len; c++)
	{
		auto chr = buff[c];
		if (chr == '{' || chr == '-' || chr == '}')
		{
			continue;
		}
		strGuid[index++] = chr;
	}
	strGuid[index] = '\0';
	_wcslwr_s(strGuid, 33);
	return ref new String(strGuid);
}

String ^ scwin::utils::SysInfoHelpers::WinRtTypeToJsonType(Type ^ type)
{
	// see https://www.json.org/
	if (type == String::typeid)
		return "string";
	if (type == Object::typeid)
		return "object";
	if (type == int::typeid)
		return "int";
	if (type == UINT::typeid)
		return "uint";
	if (type == float::typeid)
		return "float";
	if (type == double::typeid)
		return "double";
	if (type == DateTime::typeid)
		return "datetime";
	return "string";
}

Type ^ scwin::utils::SysInfoHelpers::JsonTypeToWinRtType(String ^ jsonType)
{
	// see https://www.json.org/
	if (jsonType == "string")
		return String::typeid;
	if (jsonType == "int")
		return int::typeid;
	if (jsonType == "uint")
		return UINT::typeid;
	if (jsonType == "float")
		return float::typeid;
	if (jsonType == "double")
		return double::typeid;
	if (jsonType == "datetime")
		return DateTime::typeid;
	if (jsonType == "object")
		return Object::typeid;
	return String::typeid;
}

bool scwin::utils::SysInfoHelpers::ShowNetworkErrorMessageDialog()
{
	using namespace Windows::UI::Popups;
	if (!IsInternetAvailable())
	{
		auto dlg = ref new MessageDialog("There is no internet connectivity. Please retry.", "Network Error");
		dlg->Commands->Append(ref new UICommand("Ok"));
		dlg->ShowAsync();
		return false;
	}
	return true;
}

bool scwin::utils::SysInfoHelpers::ShowNotImplementedMessageDialog(String^ title)
{
	using namespace Windows::UI::Popups;
	String^ t = StringHelpers::IsNullOrEmpty(title) ? "Not Implemented" : title;
	auto dlg = ref new MessageDialog("This feature is in the process of being implemented. Sorry for any inconvenience", t);
	dlg->Commands->Append(ref new UICommand("Ok"));
	dlg->ShowAsync();
	return false;
}
