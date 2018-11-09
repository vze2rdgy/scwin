#pragma once

namespace scwin
{

#define GETDOCURL L"data" // data/1e46d0590cebe2796402c1b2f37796c7

	class HttpHelpers
	{
	public:
		static bool IsOk(UINT code);

		static bool IsOkResponse(JsonObject^ response);

		static bool Failed(JsonObject^ remData, JsonObject^& odata);
		
		static UINT Succeeded(JsonObject^ remData, JsonValue^& odata);

		static void PrintJsonValueType(IJsonValue^ val);

		// Returns a uri by combining it with remote host root uri
		static Uri^ GetCombinedUri(String^ part);

		static String^ GetHttpResponse(Uri^ uri, cancellation_token cancelToken);

		static String ^ PostData(Uri ^ uri, String ^ postData, cancellation_token cancelToken, bool isput);

	};


}

