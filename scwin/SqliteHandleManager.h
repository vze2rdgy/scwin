#pragma once


#include <winsqlite\winsqlite3.h>
#include <concurrent_unordered_map.h>
#include <atomic>



namespace scwin
{
	struct sqlite3ex
	{
		sqlite3* dbHandle;
		atomic_int transactionCount;
		atomic_int recurseCount;
	};

	typedef concurrent_unordered_map<DWORD, sqlite3ex*> DBHANDLES;
	typedef std::unordered_map<std::string, std::string> CDBROW, CONDITIONS;

	int _stdcall sqliteExecCallback(void* p, int argc, char** argv, char** azColName);

	typedef std::function<int(void* p, int argc, char** argv, char** azColName)> SqliteCallback;

	class SqliteHandleManager
	{
		char* dbPath;
		static DBHANDLES dbHandles;

	private:
		void setDataPath();

	public:
		concurrency::critical_section cs;

	public:
		SqliteHandleManager();
		~SqliteHandleManager();



		void Shutdown();

		task<void> DeleteDb();
		sqlite3ex* OpenDb();
		void CloseDb(sqlite3ex*);
		void CloseDbs();
		sqlite3ex* GetDbHandle();

		// return handle currently active in the current thread.
		sqlite3ex* GetCurrentHandle();

		int CountRows(const std::string& table, const std::string& condition);
		int ExecSql(const std::string& sql, std::function<void(const CDBROW&)> cb = nullptr);
		void PrintState();

	};
}

