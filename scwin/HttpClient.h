#pragma once

#include "UserContext.h"
#include "UserProfile.h"
#include "UserRole.h"
#include "ObservableList.h"

using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
namespace f = Windows::Foundation;

namespace scwin
{
	namespace http
	{

#define contenttype "application/json"


		/// Remote db connection status.
		[Flags]
		public enum class ServerStatus : uint32
		{
			Disconnected = 0x1,
			Connected = 0x2,
			Synching = Connected | 0x4
		};


		//interface struct scIDataService
		//{
		//	bool Connect();
		//	bool Disconnect();
		//	property ServerStatus ConnectionStatus
		//	{
		//		ServerStatus get();
		//	}

		//	/*
		//	Verify if a user context exists in the device and associated context
		//	at the server.
		//	*/
		//	IAsyncOperation<UserContext^>^ VerifySession();
		//	IAsyncOperation<UserContext^>^ Signup(String^ email, String^ pwd, String^ company, String^ fullName);
		//	IAsyncOperation<UserContext^>^ Signin(String^ email, String^ pwd);
		//	IAsyncOperation<bool>^ SignOut();

		//	// Methods for roles
		//	IAsyncOperation<ObservableList<UserRole^>^>^ ListRoles();
		//	IAsyncOperation<bool>^ UpsertRole(UserRole^ role);
		//	IAsyncOperation<bool>^ RemRole(String^ roleName);

		//	// Methods for users
		//	IAsyncOperation<ObservableList<UserProfile^>^>^ ListUsers();
		//	IAsyncOperation<bool>^ UpsertUser(UserProfile^ role, String^ password);
		//	IAsyncOperation<bool>^ RemUser(String^ emailAddress);

		//};

		static class ServiceHttpClient
		{
		public:
			static HttpClient^ CreateHttpClient();

			static f::Uri^ GetUri(String^ urlStr);

			/// <summary>
			/// This method is useful to send a lightweight request to the server to get header information
			/// such as obtain _rev field in a couchdb document or couchdb server ping state.
			/// </summary>
			static task<String^> MakeHeadRequest(String^ url);
		};
	}
}