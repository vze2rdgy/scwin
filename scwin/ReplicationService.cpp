#include "pch.h"
#include "ReplicationService.h"
#include "BackgroundTaskHelpers.h"
#include "HttpHelpers.h"
#include "SysInfoHelpers.h"
#include <sstream>
#include "cpplinq.hpp"
#include "DateTimeHelpers.h"
#include "HashingHelpers.h"
#include "cpplinq.hpp"

// https://docs.microsoft.com/en-us/windows/uwp/data-access/sqlite-databases

using namespace scwin::services;

#define REPLICATORBGTASKNAME L"replicatorservicetask"
#define REPLICATORBGTASKENTRYPOINT L"scwin.services.replicatorservicetask"

#define REPLICATIONURL L"replication/_changes?since=%s&feed=normal&heartbeat=300000&style=all_docs&active_only=true" // initial_sync=true
#define GETALLDOCURL L"data/_all_docs?include_docs=true" // POST {"keys":["1e46d0590cebe2796402c1b2f3778c15", "1e46d0590cebe2796402c1b2f37796c7"]}


#ifdef _DEBUG
#define TIMERINTERVAL 300000000	// 30secs (60 * 10000000 for 1 min).
#else
#define TIMERINTERVAL 100000000	// 10secs (60 * 10000000 for 1 min).
#endif

ReplicatorService::ReplicatorService(scRemoteDataService* remoteDataService) :
	rds(remoteDataService)
{
	dbTypes = {
		L"role",
		L"user"
	};

	netconn = NetworkInformation::GetInternetConnectionProfile();
	NetworkInformation::NetworkStatusChanged += ref new NetworkStatusChangedEventHandler([this](Object^ sender)
	{
		this->OnNetworkStatusChange(sender);
	});



	//auto aop = BackgroundExecutionManager::RequestAccessAsync();
	//auto thentask = create_task(aop);
	//thentask.then([this](BackgroundAccessStatus status)
	//{
	//	IBackgroundTrigger^ trigger = ref new SystemTrigger(SystemTriggerType::InternetAvailable | SystemTriggerType::NetworkStateChange, false);
	//	triggerReg = scwin::BackgroundTaskHelpers::RegisterBackgroundTask(REPLICATORBGTASKENTRYPOINT, REPLICATORBGTASKNAME, trigger, nullptr);
	//	triggerReg->Progress += ref new BackgroundTaskProgressEventHandler([this](Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskProgressEventArgs^ args)
	//	{
	//		this->OnProgress(task, args);
	//	});
	//	triggerReg->Completed += ref new BackgroundTaskCompletedEventHandler([this](Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args)
	//	{
	//		this->OnCompleted(task, args);
	//	});
	//}, task_continuation_context::use_current());

	//httpClient = scwin::http::ServiceHttpClient::CreateHttpClient();

	this->CreateTimer();
}


ReplicatorService::~ReplicatorService()
{
	scwin::BackgroundTaskHelpers::UnregisterBackgroundTask(REPLICATORBGTASKNAME);
}

Uri ^ ReplicatorService::GetUri(String ^ urlStr)
{
	auto trimmed = scwin::utils::StringHelpers::Trim(urlStr);
	Uri^ uri = ref new f::Uri(CDBHOST);
	uri = uri->CombineUri(trimmed);
	OutputDebugString(uri->AbsoluteUri->Data());
	OutputDebugString(L"\n");
	return uri;
}

String^ ReplicatorService::GetResponse(Uri^ uri, cancellation_token cancelToken)
{
	return HttpHelpers::GetHttpResponse(uri, cancelToken);
}

String ^ scwin::services::ReplicatorService::PostData(Uri ^ uri, String ^ postData, cancellation_token cancelToken)
{
	return HttpHelpers::PostData(uri, postData, cancelToken, false);
}


task<void> scwin::services::ReplicatorService::SynchronizeChanges(const STRINGLIST& ids)
{
	return task<void>();
}

task<void> scwin::services::ReplicatorService::Log(LoggingLevel lvl, String ^ msg)
{
	//TODO: Invoke a reusable logger object's method from
	// here. See microsoft sample.
	return task<void>();
}


void scwin::services::ReplicatorService::Notify(const std::wstring & typeName, const std::wstring json)
{
	auto task = create_task([this, &typeName, &json]()->void
	{
		for (auto f : subscribers)
		{
			f(typeName, json);
		}
	});
}

void scwin::services::ReplicatorService::OnTimerTick(ThreadPoolTimer ^ timer)
{

	//GetDbMgr().GetDbHandler().CloseDbs();
	//GetDbMgr().GetDbHandler().PrintState();

	if (status != ReplicationStatus::Completed)
		return;


	if (GetDbMgr().replicationInProgress)
		return;

	OutputDebugString(L"ReplicatorService::OnTimerTick(ThreadPoolTimer ^ timer) is called.");
	// fetch changes if network is available.
	auto chkTask = this->CheckInternetConnectedAsync();
	chkTask.then([this](bool isConnected)
	{
		if (isConnected)
		{
			OutputDebugString(L"ReplicatorService::OnTimerTick -> Network is connected...");
			if (!GetDbMgr().replicationInProgress)
			{
				GetDbMgr().replicationInProgress = true;
				auto changes = this->GetDbMgr().FetchLocalChanges();
				if (changes && changes->Size)
				{
					// construct appropriate couchdb structure for bulk updates
					JsonObject^ obj = ref new JsonObject();
					obj->SetNamedValue("docs", changes);
					cancellation_token_source cancelsrc;
					cancellation_token tok = cancelsrc.get_token();

					OutputDebugString(L"Sending local changes to the server.\n");
					OutputDebugString(obj->ToString()->Data());

					String^ strResponse = this->PostData(this->GetUri(L"data/_bulk_docs"), obj->ToString(), tok);

					OutputDebugString(L"\nServer Responded:\n");
					OutputDebugString(strResponse->Data());

					/*
					[{"id":"mynewid","error":"conflict","reason":"Document update conflict."},{"ok":true,"id":"mynewid_1","rev":"1-3e144d4e5ea5bba405d368d8985e0b66"}]
					*/

					JsonArray^ jsresponse;
					if (JsonArray::TryParse(strResponse, &jsresponse))
					{
						if (jsresponse->ValueType == JsonValueType::Array)
						{
							//this->CleanupFailedDocuments(changes, jsresponse);
							this->SynchronizeDocuments(changes, jsresponse);
						}
					}
				}
				this->FetchUpdateChanges();
				GetDbMgr().replicationInProgress = false;
			}
		}
		else
		{
			OutputDebugString(L"ReplicatorService::OnTimerTick -> Network is NOT connected...");
		}
	});

}

std::vector<JsonObject^> scwin::services::ReplicatorService::GetDocumentTypes(const vector<string>& keys)
{
	using namespace cpplinq;
	if (!keys.empty())
	{
		JsonArray^ postedKeys = ref new JsonArray();
		for (auto str : keys)
		{
			postedKeys->Append(JsonValue::CreateStringValue(StringHelpers::ConvertToString(str.c_str())));
		}
		cancellation_token_source source;
		auto tok = source.get_token();
		JsonObject ^postedData = ref new JsonObject();
		postedData->SetNamedValue("keys", postedKeys);
		auto uri = GetUri("data/_design/scviews/_view/doctype");
		OutputDebugString(uri->AbsoluteUri->Data());
		auto strPosted = postedData->ToString();
		auto response = HttpHelpers::PostData(uri, strPosted, tok, false);
		JsonObject^ output;
		if (JsonObject::TryParse(response, &output))
		{
			if (output->HasKey("error"))
			{
				LogRetException(MessageIds::MissingValuesError, "Document Types are not previously stored in the system. This issue may be temporary until replication saves this information.");
			}
			else
			{
				auto rows = output->GetNamedArray("rows", nullptr);
				if (rows)
				{
					auto vrows = to_vector(rows);
					return std::move(from(vrows) >> select([](IJsonValue^ item)
					{
						auto obj = item->GetObject();
						return obj->GetNamedObject("value");
					}) >> to_vector());
				}
				else
					LogRetException(MessageIds::MissingValuesError, "Document Types are not previously stored in the system. This issue may be temporary until replication saves this information.");
			}
		}
	}
	return {};
}

//SynchronizeDocuments
//task<void> scwin::services::ReplicatorService::CleanupFailedDocuments(JsonArray^ send, JsonArray ^ docs)
void scwin::services::ReplicatorService::SynchronizeDocuments(JsonArray^ send, JsonArray ^ docs)
{
	using namespace cpplinq;

	auto vdocs = to_vector(docs);
	auto keys = from(vdocs) >> select([](IJsonValue^ val)
	{
		return STRCONV(val->GetObject()->GetNamedString("id"));
	}) >> to_vector();

	auto docTypes = this->GetDocumentTypes(keys);
	for (auto doc : docTypes)
	{
		auto id = doc->GetNamedString("id");
		auto rev = doc->GetNamedString("rev");
		auto type = doc->GetNamedString("type", nullptr);
		if (!StringHelpers::IsNullOrEmpty(id) && !StringHelpers::IsNullOrEmpty(rev) && !StringHelpers::IsNullOrEmpty(type))
		{
			dbMgr.UpdateRowRevisionNumber(STRCONV(id), STRCONV(rev), STRCONV(type));
		}
	}
}

void scwin::services::ReplicatorService::FetchUpdateChanges(bool dbcleared)
{
	if (dbMgr.AcquireWriteLock())
	{
		cancellation_token_source source;
		cancellation_token tok = source.get_token();
		String^ url = ref new String(REPLICATIONURL);
		if (dbcleared)
		{
			url = url + "&initial_sync=true";
		}
		url = StringHelpers::Replace(url, L"%s", StringHelpers::IsNullOrEmpty(lastSeq) ? "0" : lastSeq);
		try
		{
			String^ strRsp = this->GetResponse(GetUri(url), tok);
			// build json object from string.
			if (strRsp && !strRsp->IsEmpty())
			{
				OutputDebugString(L"Replication Data: ");
				OutputDebugString(strRsp->Data());
				OutputDebugString(L"\n");

				JsonObject^ jo;
				if (JsonObject::TryParse(strRsp, &jo))
				{
					auto results = jo->GetNamedArray(L"results");
#if COUCHDBVER == 2
					this->lastSeq = jo->GetNamedString(L"last_seq", "0");
#else
					this->lastSeq = StringHelpers::ConvertToString(std::to_string(jo->GetNamedNumber(L"last_seq", 0)).c_str());
					lastSeq = CleanupSeqNumber(lastSeq);
#endif
					const int batchSize = 100;
					int index = 0;
					JSOBJQUEUE queue;
					std::div_t q;
					q = std::div(results->Size, batchSize);
					for (int batch = 0; batch < q.quot; batch++)
					{
						index = batch * batchSize;
						auto lastItem = index + batchSize;
						while (index < lastItem)
						{
							auto jval = results->GetAt(index);
							queue.push(jval);
							index++;
						}
						// batch is done, submit
						SubmitRemoteDataInBatches(queue);
						queue.clear();
					}
					index = q.quot * batchSize;
					auto lastItem = index + q.rem;
					while (index < lastItem)
					{
						auto jval = results->GetAt(index);
						queue.push(jval);
						index++;
					}
					// batch is done, submit
					SubmitRemoteDataInBatches(queue);
					queue.clear();
				}
			}
			SetSettingsValue<string>("LASTSEQNUM", STRCONV(this->lastSeq), false);
		}
		catch (COMException^ ex)
		{
			// nothing can be done, ignore.
			LogException("Failed to fetch data from the server. Operation timed out.");
		}
	}
	dbMgr.UnlockWriteLock();
}

String ^ scwin::services::ReplicatorService::CleanupSeqNumber(String ^ seq)
{
#if COUCHDBVER == 2
	return seq;
#else
	auto decPos = StringHelpers::IndexOf(seq, ".");
	if (decPos == -1)
		return seq;
	return StringHelpers::Substring(seq, 0, decPos - 1);
#endif
}

void scwin::services::ReplicatorService::SubmitRemoteDataInBatches(JSOBJQUEUE & queue)
{
	// create a json object for __alldocs url
	std::unordered_set<String^> deletedItems;
	JsonArray^ docIds = ref new JsonArray();
	IJsonValue^ js;
	while (queue.try_pop(js))
	{
		auto obj = js->GetObject();
		String^ id = obj->GetNamedString("id");
		if (obj->GetNamedBoolean("deleted", false))
		{
			deletedItems.insert(id);
		}
		docIds->Append(JsonValue::CreateStringValue(id));
	}

	dbMgr.PurgeDeletedItems(deletedItems);

	cancellation_token_source src;
	cancellation_token tok = src.get_token();
	JsonObject^ prms = ref new JsonObject();
	prms->SetNamedValue("keys", docIds);
	auto response = this->PostData(this->GetUri(GETALLDOCURL), prms->ToString(), tok);
	// add the document to the cachedb.
	/*
	{"total_rows":260, "offset" : 0, "rows" : [
	{"id":"companyprofile", "key" : "companyprofile", "value" : {"rev":"1-8705c90a32b9339ae95290321add574f"}, "doc" : {"_id":"companyprofile", "_rev" : "1-8705c90a32b9339ae95290321add574f", "type" : "profile", "businessname" : "Empire LLC"}},
	{ "id":"6a04180de2043b40d63ea95e300002c6","key" : "6a04180de2043b40d63ea95e300002c6","value" : {"rev":"3-8f38b21732e7de00569c6c57764c53da"},"doc" : {"_id":"6a04180de2043b40d63ea95e300002c6","_rev" : "3-8f38b21732e7de00569c6c57764c53da","owner" : "charan@empire.com"} },
	{ "id":"roles","key" : "roles","value" : {"rev":"3-efbdddc8b5aadfd65effda47682cf8f3"},"doc" : {"_id":"roles","_rev" : "3-efbdddc8b5aadfd65effda47682cf8f3","roles" : ["helper","salesrep","administrator"]} },
	{ "id":"mycostomdocument","key" : "mycostomdocument","value" : {"rev":"2-cab58577296a44521831000c3d776844"},"doc" : {"_id":"mycostomdocument","_rev" : "2-cab58577296a44521831000c3d776844","owner" : "charan@empire.com"} },
	{ "id":"6a04180de2043b40d63ea95e3002011e$details","key" : "6a04180de2043b40d63ea95e3002011e$details","value" : {"rev":"5-a5fc83d408a1a5aaeaab80e5a553e004","deleted" : true},"doc" : null },
	]}
	*/


	JsonObject^ rdata;
	if (JsonObject::TryParse(response, &rdata))
	{
		// how many rows ?
		if (rdata->GetNamedNumber("total_rows") > 0)
		{
			auto rows = rdata->GetNamedArray("rows");
			this->dbMgr.SubmitData(rows);
		}
	}
}

task<bool> scwin::services::ReplicatorService::CheckInternetConnectedAsync()
{
	return create_task([]() -> bool
	{
		return scwin::utils::SysInfoHelpers::IsInternetAvailable();
	});
}

void scwin::services::ReplicatorService::OnNetworkStatusChange(Object ^ sender)
{
	auto chkTask = this->CheckInternetConnectedAsync();
	chkTask.then([this](bool isConnected)
	{
		if (isConnected)
		{
			if (timer == nullptr)
			{
				this->CreateTimer();
			}
		}
		else
		{
			if (timer != nullptr)
			{
				timer->Cancel();
				timer = nullptr;
			}
		}
	});
}


IAsyncOperationWithProgress<ReplicationStatus, int>^ scwin::services::ReplicatorService::Begin(UserContext^ userctxt, bool isjustsignin)
{
	return create_async([this, userctxt, isjustsignin](progress_reporter<int> reporter, cancellation_token token)
	{
		if (this->status == ReplicationStatus::NotStarted)
		{
			reporter.report(1);
			this->status = ReplicationStatus::Progressing;
			try
			{
				/* Three reasons for clearing db
				1. A new user signs up and the key changes
				2. Another user of a same company signs in and the he shouldn't see stuff he should not.
				3. The application administrations have made changes to the database and there a new version number is issued.
				*/
				bool clearDb = false;

				auto lastSeqNum = GetSettingsValue("LASTSEQNUM");
#if COUCHDBVER == 1
				lastSeqNum = lastSeqNum.substr(0, lastSeqNum.find("."));
#endif
				if (lastSeqNum == "0")
				{
					clearDb = true;
				}

				// check database key 
				auto dbcode = GetSettingsValue("dbcode"); // dbcode is dbcode(version#)
				// what is user context's dbcode
				if (dbcode.empty() || dbcode != STRCONV(userctxt->Key))
				{
					clearDb = true;
				}
				else if (isjustsignin)
				{
					// check if last signed in user information is different
					auto lastsignedIn = GetSettingsValue("lastuser");
					if (lastsignedIn.empty() || lastsignedIn != STRCONV(userctxt->Name))
					{
						clearDb = true; // TODO: only certain tables are cleared. will comeback on this.
					}
				}

				//clearDb = true;

				//dbMgr.Authenticate() ? how to get password after VerifySEssion ?
				//dbMgr.Encrypt() // creating a key ?
				if (clearDb)
				{
					dbMgr.ClearDb();
				}

				if (!dbMgr.CreateDataSchema())
				{
					reporter.report(100);
					Log(LoggingLevel::Error, "Failed to create cache data structure");
					this->status = ReplicationStatus::NotStarted;
					return this->status;
				}

				string lastUser = STRCONV(userctxt->Name);
				//auto dt = DateTimeHelpers::GetLocalCurrent();
				//DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("day month year hour minute timezone");
				//auto strdate = dateFormatter->Format(dt, "UTC");
				//string lastsignindt = StringHelpers::Convert(strdate);

				SetSettingsValue<string>("dbcode", STRCONV(userctxt->Key), false);
				SetSettingsValue("lastuser", lastUser, false);
				SetSettingsValue("lastloginon", "DATETIME('now')", false, true);
				SetSettingsValue("usercrccode", HashingHelpers::GetCRC32(STRCONV(userctxt->Key)), true, true);

				// start replication steps.
				auto wasset = GetSettingsValue("LASTSEQNUM");
#if COUCHDBVER == 1
				wasset = wasset.substr(0, wasset.find("."));
#endif
				this->lastSeq = StringHelpers::ConvertToString(wasset.c_str());
				try
				{
					int progress = 1;
					this->FetchUpdateChanges(clearDb);
					progress = 90;
					reporter.report(progress);
				}
				catch (const std::exception& ex)
				{
					LogException(ex.what());
					reporter.report(100);
					this->status = ReplicationStatus::NotStarted;
					Log(LoggingLevel::Error, "Failed to fetch changes from remote server and update location cache.");
					return this->status;
				}
				this->status = ReplicationStatus::Completed;
				reporter.report(100);
				dbMgr.setUser(userctxt);
				scMainViewModel^ vm = scMainViewModel::Instance;
				vm->AuthenticationStage = LandingContentStage::Dashboard;
			}
			catch (const std::exception& ex)
			{
				LogException(ex.what());
				reporter.report(100);
				this->status = ReplicationStatus::NotStarted;
				Log(LoggingLevel::Error, "Failed to open local cache database.");
				PubSubService::Instance->Publish(nullptr, PubSubMessageIds::ApplicationErrorStatusMessage, "Failed to open local cache database.");
				return this->status;
			}

		}
		return this->status;
	});
}

IAsyncOperationWithProgress<ReplicationStatus, int>^ scwin::services::ReplicatorService::End()
{
	return create_async([this](progress_reporter<int> reporter, cancellation_token token)
	{
		if (this->status == ReplicationStatus::Completed)
		{
			this->status = ReplicationStatus::NotStarted;
		}
		return status;
	});
}

ReplicationStatus scwin::services::ReplicatorService::GetStatus()
{
	return status;
}



void scwin::services::ReplicatorService::OnBackgroundTimeTrigger()
{
	OutputDebugString(L"ReplicatorService::OnBackgroundTimeTrigger is called.");
}

void scwin::services::ReplicatorService::Subscribe(std::function<void(const std::wstring&, const std::wstring&)> handler)
{
	subscribers.push_back(handler);
}


void scwin::services::ReplicatorService::CreateTimer()
{
	if (timer != nullptr)
	{
		timer->Cancel();
	}

	TimeSpan span;
	//span.Duration = 150000000; // 15 seconds
	//span.Duration = 60 * 10000000; // one min
	span.Duration = TIMERINTERVAL;
	//https://docs.microsoft.com/en-us/windows/uwp/threading-async/create-a-periodic-work-item
	timer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler([this](ThreadPoolTimer^ timer)
	{
		this->OnTimerTick(timer);
	}), span); // 1 minute interval ( 1 * 60 * 10^9)
}

void scwin::services::ReplicatorService::OnProgress(Windows::ApplicationModel::Background::BackgroundTaskRegistration ^ task, Windows::ApplicationModel::Background::BackgroundTaskProgressEventArgs ^ args)
{

}

void scwin::services::ReplicatorService::OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration ^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs ^ args)
{

}


bool scwin::services::ReplicatorService::IsSqlOk(int result)
{
	return result == SQLITE_OK;
}



