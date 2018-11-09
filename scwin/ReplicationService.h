#pragma once

#include <string>
#include <winsqlite\winsqlite3.h>
#include <map>
#include "HttpClient.h"
#include <unordered_set>
#include "scRemoteDataService.h"
#include "SqliteDataManager.h"
#include <concurrent_queue.h>

using namespace Windows::System::Threading;
using namespace Windows::Networking::Connectivity;


namespace scwin
{
	namespace services
	{
		/*
		ReplicatorService: Opens a timed connection (later will switch to websockets when
		remote streetproxy forwards couchdb websocket) and queries at intervals of changes in the remote
		server for this user.
		ReplicatorService is a singleton initialized. Implements change notification.
		*/

		typedef std::list<std::function<void(const std::wstring&, const std::wstring&)>> REPLSUBS;
		typedef std::unordered_set<std::wstring> DATATYPES;
		typedef std::vector<std::wstring> STRINGLIST;
		typedef concurrency::concurrent_queue<IJsonValue^> JSOBJQUEUE;

		public enum class ReplicationStatus
		{
			NotStarted,
			Progressing,
			Completed
		};

		class ReplicatorService
		{
		private:
			SqliteDataManager dbMgr;
			BackgroundTaskRegistration^ triggerReg;
			REPLSUBS subscribers;
			CoreDispatcher^ uidisp;
			ThreadPoolTimer^ timer;
			//HttpClient^ httpClient;
			ConnectionProfile^ netconn;
			DATATYPES dbTypes;
			String^ lastSeq ;
			scRemoteDataService* rds;
			ReplicationStatus status = ReplicationStatus::NotStarted;

		public:
			ReplicatorService(scRemoteDataService* remoteDataService);
			~ReplicatorService();

			std::string GetSettingsValue(const std::string& key)
			{
				return GetDbMgr().GetSettingsValue(key);
			}

			template<typename TVal>
			void SetSettingsValue(const std::string& key, const TVal& val, bool isRoaming, bool unquotedvalue = false)
			{
				return GetDbMgr().SetSettingsValue(key, val, isRoaming, unquotedvalue);
			}

			SqliteDataManager& GetDbMgr() 
			{ 
				return this->dbMgr; 
			}

		private:
			
			void CreateTimer();

			void OnProgress(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskProgressEventArgs^ args);

			void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args);
			
			bool IsSqlOk(int result);

			void Notify(const std::wstring& typeName, const std::wstring json);

			void OnTimerTick(ThreadPoolTimer^ timer);

			// Returns document type information of selected documents from couchdb server
			// returns nullptr if nothing is returned.
			// Each array member looks like this ["id" : "xxxx", "rev":"xxxx", "type" = "tablename"]
			std::vector<JsonObject^> GetDocumentTypes(const vector<string>& keys);

			//task<void> CleanupFailedDocuments(JsonArray^ send, JsonArray^ obj);
			void SynchronizeDocuments(JsonArray^ send, JsonArray^ obj);

			// progress is a percent number between 0 and 100
			void FetchUpdateChanges(bool dbcleared = false);

			String^ CleanupSeqNumber(String^ seq);

			void SubmitRemoteDataInBatches(JSOBJQUEUE& queue);

			task<bool> CheckInternetConnectedAsync();

			void OnNetworkStatusChange(Object^ sender);

			Uri^ GetUri(String^ urlStr);

			String^ GetResponse(Uri^ uri, cancellation_token cancelToken);

			String^ PostData(Uri^ uri, String^ postData, cancellation_token canelToken);

			// Pull all documents reported as changed
			// by couchdb and synchronize them with 
			// local cache.
			task<void> SynchronizeChanges(const STRINGLIST& ids);

			task<void> Log(LoggingLevel lvl, String^ msg);

		public:
			// Start replication service.
			IAsyncOperationWithProgress<ReplicationStatus, int>^ Begin(UserContext^ userctxt, bool isjustsignin = true);

			// End replication service.
			IAsyncOperationWithProgress<ReplicationStatus, int>^ End();

			ReplicationStatus GetStatus();

			void OnBackgroundTimeTrigger();

			void Subscribe(std::function<void (const std::wstring&, const std::wstring&)> handler);

		};

	}
}