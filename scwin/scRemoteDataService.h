#pragma once

#include "HttpClient.h"
#include "NotifierT.h"

namespace f = Windows::Foundation;


namespace scwin
{

	class scRemoteDataService sealed  //: public Notifier //, public scIDataService
	{
	private:
		Uri^ baseUrl;
		//HttpClient^ httpClient;
		critical_section lock;

	private:
		Uri^ GetUri(String^ urlStr);
		
		String^ GetResponse(Uri^ uri, cancellation_token cancelToken);

		String^ PostData(Uri^ uri, String^ postData, cancellation_token canelToken, bool isput = false);


	public:
		scRemoteDataService();

		virtual ~scRemoteDataService();

		virtual bool Connect();

		virtual bool Disconnect();

		virtual task<UserContext^> VerifySession();
		virtual task<UserContext^> Signup(String^ email, String^ pwd, String^ company, String^ fullName);
		virtual task<UserContext^> Signin(String^ email, String^ pwd);
		virtual IAsyncOperation<bool>^ SignOut();

		// Methods for roles
		//virtual IAsyncOperation<IUserRoleList^>^ ListRoles();
		task<IVector<UserRole^>^> ListRolesAsync();

		virtual task<bool> UpsertRole(UserRole^ role);
		virtual IAsyncOperation<bool>^ RemRole(String^ roleName);

		// Methods for users
		//virtual IAsyncOperation<IUserProfileList^>^ ListUsers();
		
		task<IUserProfileList^> ListUsersAsync();
		virtual task<bool> UpsertUser(UserProfile^ role, String^ password);
		virtual IAsyncOperation<bool>^ RemUser(String^ emailAddress);
		task<MessageIds> ResetUserPassword(String^ userId);
	
		task<IUserProfileList^> ListUsersByRoleAsync(String^ roleName);
	
	};
}