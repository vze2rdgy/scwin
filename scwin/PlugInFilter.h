#pragma once

namespace scwin
{
	namespace http
	{
		ref class PlugInFilter sealed :
			public Windows::Web::Http::Filters::IHttpFilter
		{
		private:
			Windows::Web::Http::Filters::IHttpFilter^ innerFilter;

		public:
			PlugInFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter);
			virtual ~PlugInFilter();
			virtual Windows::Foundation::IAsyncOperationWithProgress<Windows::Web::Http::HttpResponseMessage^, Windows::Web::Http::HttpProgress>^ SendRequestAsync(Windows::Web::Http::HttpRequestMessage^ request);

		};
	}
}