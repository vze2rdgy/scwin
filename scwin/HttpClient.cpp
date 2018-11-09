#include "pch.h"
#include "HttpClient.h"

HttpClient ^ scwin::http::ServiceHttpClient::CreateHttpClient()
{
	auto baseUrl = ref new Uri(CDBHOST);
	using namespace Windows::Web::Http::Headers;
	auto filter = ref new HttpBaseProtocolFilter();
	filter->CacheControl->ReadBehavior = HttpCacheReadBehavior::Default;
	filter->CacheControl->WriteBehavior = HttpCacheWriteBehavior::Default;
	//filter = ref new PlugInFilter(filter);
	auto httpClient = ref new HttpClient(filter);
	auto packageId = Windows::ApplicationModel::Package::Current->Id;
	httpClient->DefaultRequestHeaders->UserAgent->Append(
		ref new HttpProductInfoHeaderValue(packageId->FullName, packageId->Version.ToString())
	);
	httpClient->DefaultRequestHeaders->Append("Accept", contenttype);
	return httpClient;
}

f::Uri ^ scwin::http::ServiceHttpClient::GetUri(String ^ urlStr)
{
	auto trimmed = scwin::utils::StringHelpers::Trim(urlStr);
	f:Uri^ uri = ref new f::Uri(CDBHOST);
	uri = uri->CombineUri(trimmed);
	OutputDebugString(uri->AbsoluteUri->Data());
	return uri;
}

task<String^> scwin::http::ServiceHttpClient::MakeHeadRequest(String ^ url)
{
	return create_task([url]() -> String^
	{
		auto httpClient = CreateHttpClient();
		HttpRequestMessage^ message = ref new HttpRequestMessage(HttpMethod::Head, GetUri(url));
		HttpResponseMessage^ response = httpClient->SendRequestAsync(message)->GetResults();
		return response->Content->ReadAsStringAsync()->GetResults();
	});
}
