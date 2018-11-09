#include "pch.h"
#include "scRemoteDataService.h"
#include "PlugInFilter.h"
#include "StringHelpers.h"
#include "HttpHelpers.h"
#include "ProfileDataBuilder.h"
#include "AdminDataBuilder.h"
#include "cpplinq.hpp"

using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Storage::Streams;
using namespace scwin;
using namespace scwin::http;
using namespace Platform::Collections;

// hardcoding for now.



Uri ^ scwin::scRemoteDataService::GetUri(String ^ urlStr)
{
	return HttpHelpers::GetCombinedUri(urlStr);
}

scRemoteDataService::scRemoteDataService()
{
	//httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
}


scRemoteDataService::~scRemoteDataService()
{

}
bool scRemoteDataService::Connect()
{
	return true;
}

bool scRemoteDataService::Disconnect()
{
	return true;
}

/* example from microsoft

IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ PlugInFilter::SendRequestAsync(
HttpRequestMessage^ request)
{
request->Headers->Insert("Custom-Header", "CustomRequestValue");

return create_async([=](progress_reporter<HttpProgress> reporter, cancellation_token token){
HttpResponseMessage^ response = nullptr;

task<HttpResponseMessage^> sendRequestTask(innerFilter->SendRequestAsync(request), token);
sendRequestTask.wait();
response = sendRequestTask.get();

if (token.is_canceled())
{
cancel_current_task();
return response;
}

response->Headers->Insert("Custom-Header", "CustomResponseValue");
return response;
});
}

*/

#define GURI scwin::http::ServiceHttpClient::GetUri
#define usercontext L"userCtx"


task<UserContext^> scwin::scRemoteDataService::VerifySession()
{
	//TODO this->ConnectionStatus before going forward.
	using namespace scwin::utils;

	return create_task([this]()->UserContext^
	{
		cancellation_token_source src;
		auto token = src.get_token();
		if (!SysInfoHelpers::IsInternetAvailable())
		{
			ThrowException(MessageIds::InternetNotAvailable);
		}
		auto uri = GURI(L"auth/verifySession");
		auto httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
		task<HttpResponseMessage^> userCtxtTask(httpClient->GetAsync(uri), token);
		try
		{
			userCtxtTask.wait();
		}
		catch (COMException^ ex)
		{
			//Below Message when server is not available.
			// A connection with the server could not be established
			throw ex; //TODO handle gracefully.
		}
		auto response = userCtxtTask.get();
		if (token.is_canceled())
		{
			cancel_current_task();
		}

		if (response)
		{
			task<String^> readContentTask(response->Content->ReadAsStringAsync(), token);
			readContentTask.wait();
			auto str = readContentTask.get();
			if (str && !str->IsEmpty())
			{
				JsonObject^ jo = JsonObject::Parse(str);
				JsonObject^ inData;
				if (!HttpHelpers::Failed(jo, inData))
				{
					if (inData)
					{
						return ProfileDataBuilder::Create(inData->GetNamedObject(usercontext));
					}
					return nullptr;
				}
			}
		}
		//throw CreateException(MessageIds::SessionVerificationError);
		return nullptr;
	});
}


task<UserContext^> scwin::scRemoteDataService::Signup(String ^ semail, String ^ spwd, String ^ scompany, String^ sfullName)
{
	//TODO this->ConnectionStatus before going forward.
	return create_task([this, semail, spwd, scompany, sfullName]() -> UserContext^
	{
		cancellation_token_source csrc;
		cancellation_token canceltoken = csrc.get_token();

		if (!SysInfoHelpers::IsInternetAvailable())
		{
			ThrowException(MessageIds::InternetNotAvailable);
		}

		auto uri = GURI(L"auth/signup");
		auto data = ProfileDataBuilder::CreateSignupInfo(semail, spwd, scompany, sfullName);
		auto httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
		task<HttpResponseMessage^> userCtxtTask(
			httpClient->PostAsync(
				uri, 
				ref new HttpStringContent(data->ToString(), UnicodeEncoding::Utf8, contenttype)), 
			canceltoken
		);
		userCtxtTask.wait();
		auto response = userCtxtTask.get();
		if (canceltoken.is_canceled())
		{
			cancel_current_task();
		}
		if (response)
		{
			task<String^> readContentTask(response->Content->ReadAsStringAsync(), canceltoken);
			readContentTask.wait();
			auto str = readContentTask.get();
			if (canceltoken.is_canceled())
			{
				cancel_current_task();
			}
			if (str && !str->IsEmpty())
			{
				JsonObject^ jo = JsonObject::Parse(str);
				JsonObject^ inData;
				if (!HttpHelpers::Failed(jo, inData))
				{
					if (inData)
					{
						return ProfileDataBuilder::Create(inData->GetNamedObject(usercontext));
					}
					return nullptr;
				}
			}
		}
		throw CreateException(MessageIds::SigningInFailed);
	});
}

task<UserContext^> scwin::scRemoteDataService::Signin(String ^ semail, String ^ spwd)
{
	return create_task([this, semail, spwd]() -> UserContext^
	{
		cancellation_token_source source;
		cancellation_token canceltoken = source.get_token();

		if (!SysInfoHelpers::IsInternetAvailable())
		{
			ThrowException(MessageIds::InternetNotAvailable);
		}

		auto uri =  GURI(L"auth/signin");
		auto data = ProfileDataBuilder::CreateSignInInfo(semail, spwd);
		auto sdata = data->ToString();
		auto httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
		task<HttpResponseMessage^> userCtxtTask(httpClient->PostAsync(
			uri, 
			ref new HttpStringContent(sdata, UnicodeEncoding::Utf8, contenttype)),
			canceltoken
		);
		userCtxtTask.wait();
		auto response = userCtxtTask.get();
		if (canceltoken.is_canceled())
		{
			cancel_current_task();
		}
		if (response)
		{
			task<String^> readContentTask(response->Content->ReadAsStringAsync(), canceltoken);
			readContentTask.wait();
			auto str = readContentTask.get();
			if (canceltoken.is_canceled())
			{
				cancel_current_task();
			}
			if (str && !str->IsEmpty())
			{
				JsonObject^ jo = JsonObject::Parse(str);
				JsonObject^ inData;
				if (!HttpHelpers::Failed(jo, inData))
				{
					if (inData)
					{
						return ProfileDataBuilder::Create(inData->GetNamedObject(usercontext));
					}
					return nullptr;
				}
			}
		}
		throw CreateException(MessageIds::SigningInFailed);
	});
}

IAsyncOperation<bool>^ scwin::scRemoteDataService::SignOut()
{
	return create_async([this](cancellation_token canceltoken) -> bool
	{
		auto uri = GURI(L"auth/signout");
		auto httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();
		task<HttpResponseMessage^> userCtxtTask(httpClient->PostAsync(uri, ref new HttpStringContent("{}", UnicodeEncoding::Utf8, contenttype)), canceltoken);
		userCtxtTask.wait();
		auto response = userCtxtTask.get();
		if (canceltoken.is_canceled())
		{
			cancel_current_task();
		}
		if (response)
		{
			task<String^> readContentTask(response->Content->ReadAsStringAsync(), canceltoken);
			readContentTask.wait();
			auto str = readContentTask.get();
			if (canceltoken.is_canceled())
			{
				cancel_current_task();
			}
			if (str && !str->IsEmpty())
			{
				JsonObject^ jo = JsonObject::Parse(str);
				JsonObject^ inData;
				if (!HttpHelpers::Failed(jo, inData))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		throw Exception::CreateException(E_FAIL, "Unable to access server. Failed to complete the signin process. Please retry.");
	});
}

String^ scRemoteDataService::GetResponse(Uri^ uri, cancellation_token cancelToken)
{
	return HttpHelpers::GetHttpResponse(uri, cancelToken);
}

String ^ scwin::scRemoteDataService::PostData(Uri ^ uri, String ^ postData, cancellation_token cancelToken, bool isput)
{
	return HttpHelpers::PostData(uri, postData, cancelToken, isput);
}


//IAsyncOperation<IUserRoleList^>^ scwin::scRemoteDataService::ListRoles()
//{
//	auto acall = create_async([this](cancellation_token canceltoken) -> IUserRoleList^
//	{
//		auto str = this->GetResponse(GURI(L"admin/roles"), canceltoken);
//		if (str && !str->IsEmpty())
//		{
//			JsonObject^ jo;
//			if (JsonObject::TryParse(str, &jo))
//			{
//				JsonValue^ oj;
//				if (!HttpHelpers::Succeeded(jo, oj))
//				{
//					if (oj)
//					{
//						HttpHelpers::PrintJsonValueType(oj);
//						return AdminDataBuilder::ConvertToUserRoles(oj->GetArray());
//					}
//					return nullptr;
//				}
//				else
//				{
//					return nullptr;
//				}
//			}
//			return nullptr;
//		}
//	});
//	return acall;
//}

task<IVector<UserRole^>^> scwin::scRemoteDataService::ListRolesAsync()
{
	return create_task([this]() -> IVector<UserRole^>^
	{
		std::lock_guard<std::recursive_mutex> g(gLock);
		cancellation_token_source tcs;
		cancellation_token tok = tcs.get_token();
		auto str = this->GetResponse(GURI(L"admin/roles"), tok);
		if (str && !str->IsEmpty())
		{
			JsonObject^ jo;
			if (JsonObject::TryParse(str, &jo))
			{
				JsonValue^ oj;
				if (!HttpHelpers::Succeeded(jo, oj))
				{
					if (oj)
					{
						HttpHelpers::PrintJsonValueType(oj);
						return AdminDataBuilder::ConvertToUserRoles(oj->GetArray());
					}
					return nullptr;
				}
				else
				{
					return nullptr;
				}
			}
			return nullptr;
		}
		return nullptr;
	});
}

task<bool> scwin::scRemoteDataService::UpsertRole(UserRole ^ role)
{
	/*

	Request:
	{
		"roles" : [
		"team leader"
		]
	}

	Response:
	{
	  "code": 200,
	  "result": [
		"TEAM LEADER",
		"HELPER",
		"ADMINISTRATOR"
	  ]
	}
	*/
	auto acall = create_task([this, role]() -> bool
	{
		JsonObject^ jo = ref new JsonObject();
		jo->SetNamedValue("id", JsonValue::CreateStringValue(role->Id));
		jo->SetNamedValue("role", JsonValue::CreateStringValue(role->Name));
		jo->SetNamedValue("functionalrights", JsonValue::CreateStringValue(StringHelpers::ConvertToString(std::to_string(role->Rights).c_str())));
		auto sdata = "{ \"roles\" : [" + jo->ToString() + "]}";
		cancellation_token_source tcs;
		cancellation_token tok = tcs.get_token();
		auto str = this->PostData(GURI(L"admin/roles"), sdata, tok, true);
		if (str && !str->IsEmpty())
		{
			JsonObject^ jo = JsonObject::Parse(str);
			JsonValue^ inData;
			auto httpStatus = HttpHelpers::Succeeded(jo, inData);
			return HttpHelpers::IsOk(httpStatus);
		}
		return false;
	});
	return acall;
}

IAsyncOperation<bool>^ scwin::scRemoteDataService::RemRole(String ^ roleName)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

//IAsyncOperation<IUserProfileList^>^ scwin::scRemoteDataService::ListUsers()
//{
//	auto acall = create_async([this](cancellation_token canceltoken) -> IUserProfileList^
//	{
//		auto str = this->GetResponse(GURI(L"admin/usrlist"), canceltoken);
//		if (str && !str->IsEmpty())
//		{
//			JsonObject^ jo;
//			if (JsonObject::TryParse(str, &jo))
//			{
//				JsonValue^ oj;
//				if (!HttpHelpers::Succeeded(jo, oj))
//				{
//					if (oj)
//					{
//						HttpHelpers::PrintJsonValueType(oj);
//						return AdminDataBuilder::ConvertToUsers(oj->GetArray());
//					}
//					return nullptr;
//				}
//				else
//				{
//					return nullptr;
//				}
//			}
//			return nullptr;
//		}
//	});
//	return acall;
//}

task<IUserProfileList^> scwin::scRemoteDataService::ListUsersAsync()
{
	return create_task([this]() -> IUserProfileList^
	{
		std::lock_guard<std::recursive_mutex> g(gLock);
		cancellation_token_source tcs;
		cancellation_token tok = tcs.get_token();
		auto str = this->GetResponse(GURI(L"admin/usrlist"), tok);
		if (str && !str->IsEmpty())
		{
			JsonObject^ jo;
			if (JsonObject::TryParse(str, &jo))
			{
				JsonValue^ oj;
				if (!HttpHelpers::Succeeded(jo, oj))
				{
					if (oj)
					{
						return AdminDataBuilder::ConvertToUsers(oj->GetArray());
					}
					return nullptr;
				}
				else
				{
					return nullptr;
				}
			}
			return nullptr;
		}
		return nullptr;
	});
}

task<bool> scwin::scRemoteDataService::UpsertUser(UserProfile ^ usr, String^ password)
{
	/*
	{
		"email": "newsalesrep4@walmart.com",
		"password": "password",
		"fullname": "rocket singh",
		"roles" : ["salesrep"]
	}

	{
	"code": 200,
	"result": "User added successfully"
	}
	*/

	auto acall = create_task([this, usr, password]()->bool
	{
		auto jo = ref new JsonObject();
		jo->SetNamedValue(L"email", JsonValue::CreateStringValue(usr->Email));
		jo->SetNamedValue(L"password", JsonValue::CreateStringValue(password));
		jo->SetNamedValue(L"fullname", JsonValue::CreateStringValue(usr->FullName));
		JsonArray^ arr = ref new JsonArray();
		if (usr->Roles != nullptr)
		{
			for (auto r : usr->Roles)
			{
				arr->Append(JsonValue::CreateStringValue(r));
			}
		}

		jo->SetNamedValue("roles", arr);
		cancellation_token_source tcs;
		cancellation_token tok = tcs.get_token();
		String^ api = usr->ObjectState == DataState::Added ? L"admin/usradd" : L"admin/usrupd";
		auto str = this->PostData(GURI(api), jo->ToString(), tok);
		if (str && !str->IsEmpty())
		{
			JsonObject^ jo = JsonObject::Parse(str);
			JsonValue^ inData;
			return HttpHelpers::IsOk(HttpHelpers::Succeeded(jo, inData));
		}
		return false;
	});
	return acall;
}

IAsyncOperation<bool>^ scwin::scRemoteDataService::RemUser(String ^ emailAddress)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

task<MessageIds> scwin::scRemoteDataService::ResetUserPassword(String ^ userId)
{
	return create_task([this, userId]() -> MessageIds
	{
		cancellation_token_source tcs;
		cancellation_token tok = tcs.get_token();
		auto str = this->PostData(GURI("admin/resetpassword"), "{ \"email\" : \"" + userId + "\" }", tok);
		JsonObject^ jo;
		if (!JsonObject::TryParse(str, &jo))
			return MessageIds::WebApiMethodFailed;
		if (HttpHelpers::IsOkResponse(jo))
			return MessageIds::Success;
		else
			return MessageIds::WebApiMethodFailed;
	});
}

task<IUserProfileList^> scwin::scRemoteDataService::ListUsersByRoleAsync(String ^ roleName)
{
	using namespace cpplinq;

	auto create_task([this, roleName]()
	{
		IUserProfileList^ newlist = ref new Vector<UserProfile^>();
		auto task = ListUsersAsync();
		task.wait();
		auto userlist = task.get();
		for (auto usr : userlist)
		{
			auto roles = usr->Roles;
			if (roles != nullptr)
			{
				auto hasany = from(to_vector(roles)) >> any([roleName](String^ rn)
				{
					return roleName == rn;
				});
				if (hasany)
					newlist->Append(usr);
			}
		}
		return newlist;
	});
	return task<IUserProfileList^>();
}


















//auto uri = GetUri(L"auth/verifySession");
//auto getTask = create_task(httpClient->GetAsync(uri)).then([=]()->UserContext^
//{
//	return nullptr;
//});

//	auto action = httpClient->GetAsync(uri);
//	action->Progress = ref new AsyncOperationProgressHandler<HttpResponseMessage^, HttpProgress>(
//		[this](IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ asyncInfo, HttpProgress progressInfo)
//	{
//		switch (progressInfo.Stage)
//		{
//		case HttpProgressStage::DetectingProxy:
//			OutputDebugString(L"Detecting Proxy");
//			break;
//		case HttpProgressStage::ConnectingToServer:
//			OutputDebugString(L"ConnectingToServer");
//			break;
//			case HttpProgressStage::NegotiatingSsl:
//			OutputDebugString(L"NegotiatingSsl");
//			break;
//		case HttpProgressStage::ReceivingContent:
//			OutputDebugString(L"ReceivingContent");
//			break;
//		case HttpProgressStage::ReceivingHeaders:
//			OutputDebugString(L"ReceivingHeaders");
//			break;
//		case HttpProgressStage::ResolvingName:
//			OutputDebugString(L"ResolvingName");
//			break;
//		case HttpProgressStage::SendingContent:
//			OutputDebugString(L"SendingContent");
//			break;
//		case HttpProgressStage::WaitingForResponse:
//			OutputDebugString(L"WaitingForResponse");
//			break;
//		}
//	});
//	action->Completed = ref new AsyncOperationWithProgressCompletedHandler<HttpResponseMessage^, HttpProgress>(
//		[this](IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ asyncInfo, HttpProgress progressInfo)
//	{
//		OutputDebugString(L"Stepping");
//		asyncInfo->Completed = ref new AsyncOperationWithProgressCompletedHandler<String^, HttpProgress>([this]
//		(IAsyncOperationWithProgress<String^, HttpProgress>^, HttpProgress progress){

//		});
//	});
//	auto result = action->GetResults();
//	auto content = result->Content->ReadAsStringAsync()->GetResults();
//	JsonValue^ js = JsonValue::Parse(content);
//	return ref new UserContext(js->GetObject());
//});
