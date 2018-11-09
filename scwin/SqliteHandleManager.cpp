#include "pch.h"
#include "SqliteHandleManager.h"

namespace scwin
{

#define CACHEDB L"sccache.db"
#define CDBNAME L"sccache"

	DBHANDLES SqliteHandleManager::dbHandles;

	SqliteHandleManager::SqliteHandleManager()
	{
		setDataPath();
		sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
		sqlite3_initialize();
	}


	SqliteHandleManager::~SqliteHandleManager()
	{
		Shutdown();
	}

	void SqliteHandleManager::Shutdown()
	{
		if (dbPath != nullptr)
		{
			CloseDbs();
			free(dbPath);
			sqlite3_shutdown();
			dbPath = nullptr;
		}
	}

	task<void> SqliteHandleManager::DeleteDb()
	{
		return create_task([this]()
		{
			auto file = create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(STRCONV(dbPath))).get();
			if (file)
				create_task(file->DeleteAsync()).get();
		});
	}

	sqlite3ex* SqliteHandleManager::OpenDb()
	{
		/* TODO: Two features to add
		(a) Utilize a larger removable disk if available to keep the db file.
		(b) Utilize user's userid/password to authenticate the database (and possibly encrypt as well if a
		removable disk is used). see doc (http://www.sqlite.org/src/doc/trunk/ext/userauth/user-auth.txt)
		*/



		// open db
		concurrency::critical_section::scoped_lock l(cs);
		sqlite3ex* hDb;
		auto dwThreadId = GetCurrentThreadId();
		OutputDebugStringA("Current Thread Id: "); OutputDebugStringA(std::to_string(dwThreadId).c_str()); OutputDebugStringA("\n");
		auto f = dbHandles.find(dwThreadId);
		if (f == dbHandles.end() || f->second == nullptr || f->second->dbHandle == nullptr)
		{
			hDb = new sqlite3ex{ nullptr, 0 };
			auto ret = sqlite3_open_v2(
				dbPath, 
				&hDb->dbHandle, 
				SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX,
				nullptr);
			if (ret != SQLITE_OK)
			{
				throw std::exception("Failed to open local cache file.");
			}
			// WAL mode (Write Ahead Logging mode)
			if (f == dbHandles.end())
				dbHandles.insert(std::make_pair(dwThreadId, hDb));
			else
				f->second = hDb;
			//ret = ExecSql("PRAGMA journal_mode=WAL;");
			ret = sqlite3_exec(hDb->dbHandle, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
			if (ret != SQLITE_OK)
			{
				throw std::exception("Failed to open local cache file.");
			}
		}
		else
		{
			hDb = f->second;
		}
		hDb->recurseCount++;
		OutputDebugStringA("Opening DB-> Sqlite connection # "); OutputDebugStringA(to_string((__int64)hDb->dbHandle).c_str()); OutputDebugStringA(" has recursion count = "); OutputDebugStringA(to_string(hDb->recurseCount).c_str()); OutputDebugStringA("\n");
		return hDb;
	}

	void SqliteHandleManager::CloseDb(sqlite3ex *h)
	{
		if (h)
		{
			concurrency::critical_section::scoped_lock l(cs);
			h->recurseCount--;
			OutputDebugStringA("Closing DB-> Sqlite connection # "); OutputDebugStringA(to_string((__int64)h->dbHandle).c_str()); OutputDebugStringA(" has recursion count = "); OutputDebugStringA(to_string(h->recurseCount).c_str()); OutputDebugStringA("\n");
			if (h->recurseCount <= 0)
			{
				auto newh = (sqlite3ex*)InterlockedExchangePointer((PVOID*)&h, nullptr);
				sqlite3_close(newh->dbHandle);
				dbHandles[GetCurrentThreadId()]->dbHandle = nullptr;
			}
		}
	}

	sqlite3ex* SqliteHandleManager::GetDbHandle()
	{
		return OpenDb();
	}


	sqlite3ex * SqliteHandleManager::GetCurrentHandle()
	{
		concurrency::critical_section::scoped_lock l(cs);
		auto dwThreadId = GetCurrentThreadId();
		auto f = dbHandles.find(dwThreadId);
		if (f == dbHandles.end() || f->second == nullptr || f->second->dbHandle == nullptr)
		{
			throw Exception::CreateException(E_FAIL, "No db connection available for the current thread.");
			//return nullptr;
		}
		return f->second;
	}

	int SqliteHandleManager::CountRows(const std::string& table, const std::string& condition)
	{
		std::ostringstream ostr;
		ostr << "select count(*) as CNT from " << table << " where " << condition;
		int count = 0;
		char* errMsg = 0;
		auto dbHandle = GetDbHandle();
		auto result = sqlite3_exec(dbHandle->dbHandle, ostr.str().c_str(), [](void* p, int argc, char** argv, char** azColName)
		{
			//int& cnt = (int)(*(int*)(p));
			(int)(*(int*)(p)) = strtol(argv[0], nullptr, 10);
			return SQLITE_OK;
		}, &count, &errMsg);
		if (result != SQLITE_OK)
		{
			OutputDebugStringA(errMsg);
			sqlite3_free(errMsg);
		}
		CloseDb(dbHandle);
		return count;
	}

	int SqliteHandleManager::ExecSql(const std::string & sql, std::function<void(const CDBROW&)> cb)
	{
		char* errMsg = 0;
		int result;
		auto dbHandle = GetDbHandle();
		if (cb)
		{
			result = sqlite3_exec(dbHandle->dbHandle, sql.c_str(), sqliteExecCallback, &cb, &errMsg);
		}
		else
		{
			result = sqlite3_exec(dbHandle->dbHandle, sql.c_str(), nullptr, nullptr, &errMsg);
		}
		if (result != SQLITE_OK)
		{
			OutputDebugStringA(errMsg);
			sqlite3_free(errMsg);
		}
		CloseDb(dbHandle);
		return result;
	}

	void SqliteHandleManager::PrintState()
	{
		stringstream ostr;
		for (auto dbhr : dbHandles)
		{
			auto tid = dbhr.first;
			auto dbh = dbhr.second;
			ostr << "Printing DB Connection States" << endl;
			ostr << "Thread Id: " << tid << endl;
			if (dbh)
			{
				ostr << "DbHandle: " << dbh->dbHandle << endl;
				ostr << "Recurse Count: " << dbh->recurseCount << endl;
				ostr << "Transaction Count: " << dbh->transactionCount << endl;
			}
			else
			{
				ostr << "No DB Handle." << endl;
			}
			ostr << "Completed." << endl;
		}
		OutputDebugStringA(ostr.str().c_str());
	}

	void SqliteHandleManager::setDataPath()
	{
		String^ path = String::Concat(String::Concat(Windows::Storage::ApplicationData::Current->LocalFolder->Path, (String^)L"\\"), (String^)CACHEDB);
		const wchar_t* wdbPath = path->Data();
		// convert to 
		size_t wstrSize = wcslen(wdbPath);
		size_t convertedChars = 0;
		size_t sizeInBytes = wstrSize + 1 * 2; // include null char
		errno_t err = 0;
		mbstate_t       mbstate;
		// Reset to initial shift state  
		::memset((void*)&mbstate, 0, sizeof(mbstate));
		dbPath = (char*)malloc(sizeInBytes);
		err = wcsrtombs_s(&convertedChars, dbPath, sizeInBytes, &wdbPath, wstrSize, &mbstate);
		if (err != 0)
		{
			throw std::exception("String conversion error");
		}
		OutputDebugStringA("\nDBPath: ");
		OutputDebugStringA(dbPath);
		OutputDebugStringA("\n");
	}

	void SqliteHandleManager::CloseDbs()
	{
		concurrency::critical_section::scoped_lock l(cs);
		for (auto h : dbHandles)
		{
			sqlite3_close(h.second->dbHandle);
			delete h.second;
		}
		dbHandles.clear();
	}

}