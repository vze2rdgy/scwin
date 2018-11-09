#include "pch.h"
#include "HttpHelpers.h"

#define status L"code"
#define result L"result"

using namespace scwin;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Storage::Streams;

bool HttpHelpers::IsOk(UINT code)
{
	if (code >= 200 && code <300)
		return true;
	return false;
}

bool scwin::HttpHelpers::IsOkResponse(JsonObject ^ response)
{
	auto code = response->GetNamedNumber(status);
	return code >= 200 && code < 300;
}

bool scwin::HttpHelpers::Failed(JsonObject^ remData, JsonObject^& dataObj)
{
	auto code = remData->GetNamedNumber(status);
	if (code >= 200 && code < 300)
	{
		auto x = remData->GetNamedObject(result);
		dataObj = x;
		return false;
	}
	auto res = remData->GetNamedString(result);
	if (res)
	{
		LogException(STRCONV(res->Data()).c_str());
	}
	return true;
}

UINT scwin::HttpHelpers::Succeeded(JsonObject ^ remData, JsonValue^& dataObj)
{
	auto code = (UINT)remData->GetNamedNumber(status);
	if (code >= 200 && code < 300)
	{
		IJsonValue^ x = remData->Lookup(result);
		dataObj = (JsonValue^)x;
		return 0;
	}	
	String^ strMsg = L"Failed remote call. Invalid response content. Returned error message: " + code;
	OutputDebugString(strMsg->Data());
	OutputDebugString(L"\n");
	return code;
}

void scwin::HttpHelpers::PrintJsonValueType(IJsonValue ^ val)
{
	if (val == nullptr)
	{
		OutputDebugString(L"Value is null");
		return;
	}
	switch(val->ValueType)
	{
	case JsonValueType::Array:
		OutputDebugString(L"Value is array");
		break;
	case JsonValueType::Boolean:
		OutputDebugString(L"Value is bool");
		break;
	case JsonValueType::Null:
		OutputDebugString(L"Value is json null");
		break;
	case JsonValueType::Number:
		OutputDebugString(L"Value is number");
		break;
	case JsonValueType::Object:
		OutputDebugString(L"Value is object");
		break;
	case JsonValueType::String:
		OutputDebugString(L"Value is string");
		break;
	}
}

Uri ^ scwin::HttpHelpers::GetCombinedUri(String ^ part)
{
	auto trimmed = scwin::utils::StringHelpers::Trim(part);
	Uri^ uri = ref new f::Uri(CDBHOST);
	uri = uri->CombineUri(trimmed);
	OutputDebugString(uri->AbsoluteUri->Data());
	return uri;
}

String ^ scwin::HttpHelpers::GetHttpResponse(Uri ^ uri, cancellation_token cancelToken)
{
	auto httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
	task<HttpResponseMessage^> t1 = create_task(httpClient->GetAsync(uri), cancelToken);
	try
	{
		t1.wait();
	}
	catch (COMException^ ex)
	{
		LogException(StringHelpers::Convert(ex->Message->Data()).c_str());
		throw ex;
	}
	if (cancelToken.is_canceled())
		cancel_current_task();
	task<String^> t2(t1.get()->Content->ReadAsStringAsync(), cancelToken);
	try
	{
		t2.wait();
	}
	catch (COMException^ ex)
	{
		//Below Message when server is not available.
		// A connection with the server could not be established
		throw ex; //TODO handle gracefully.
	}
	if (cancelToken.is_canceled())
		cancel_current_task();
	return t2.get();
}

String ^ scwin::HttpHelpers::PostData(Uri ^ uri, String ^ postData, cancellation_token cancelToken, bool isput)
{
	auto httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
	task<HttpResponseMessage^> t1;
	if (isput)
	{
		t1 = create_task(httpClient->PutAsync(
			uri, ref new HttpStringContent(postData,
				Windows::Storage::Streams::UnicodeEncoding::Utf8, contenttype)), cancelToken);
	}
	else
	{
		t1 = create_task(httpClient->PostAsync(
			uri, ref new HttpStringContent(postData,
				Windows::Storage::Streams::UnicodeEncoding::Utf8, contenttype)), cancelToken);
	}
	try
	{
		t1.wait();
	}
	catch (COMException^ ex)
	{
		//Below Message when server is not available.
		// A connection with the server could not be established
		throw ex; //TODO handle gracefully.
	}
	if (cancelToken.is_canceled())
		cancel_current_task();
	task<String^> t2(t1.get()->Content->ReadAsStringAsync(), cancelToken);
	t2.wait();
	if (cancelToken.is_canceled())
		cancel_current_task();
	return t2.get();

}

