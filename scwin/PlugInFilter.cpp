#include "pch.h"
#include "PlugInFilter.h"

using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Foundation;
using namespace Concurrency;
using namespace scwin::http;

PlugInFilter::PlugInFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter)
{
	if (!innerFilter)
	{
		throw ref new COMException(E_INVALIDARG, "innerFilter parameter is required.");
	}
	this->innerFilter = innerFilter;
}

PlugInFilter::~PlugInFilter()
{

}

IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ PlugInFilter::SendRequestAsync(HttpRequestMessage^ request)
{
	request->Headers->Insert("Custom-Header", "CustomRequestValue"); // will be used for authentication purpose.
	return create_async([&](progress_reporter<HttpProgress> reporter, cancellation_token token)
	{
		HttpResponseMessage^ response = nullptr;
		task<HttpResponseMessage^> sendReqTask(innerFilter->SendRequestAsync(request), token);
		sendReqTask.wait();
		response = sendReqTask.get();
		if (token.is_canceled())
		{
			cancel_current_task();
			return response;
		}
		response->Headers->Insert("Custom-Header", "CustomResponseValue"); // 
		return response;
	});
}
