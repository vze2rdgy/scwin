#include "pch.h"
#include "SqliteDataManager.h"
#include <winsqlite\winsqlite3.h>
#include "cpplinq.hpp"
#include <strstream>
#include "HttpHelpers.h"
#include <map>

using namespace std;

#define DISABLETRANSACTIONS 0

namespace scwin
{

#define DBSELECTOR(sql) ExecSqlNative(sql, [=](void* p, int argc, char** argv, char** azColName)
#define DBSELECTORP(sql, prm) ExecSqlNative(sql, [this, prm](void* p, int argc, char** argv, char** azColName)
#define DBSELECTORALLP(sql) ExecSqlNative(sql, [=](void* p, int argc, char** argv, char** azColName)


	typedef std::map<int, const char*> SqliteErrorMap;
	SqliteErrorMap sqliteErrors = { 
		{0, "Success"}, 
		{1, "Generic error"}, 
		{2, "Internal logic error in SQLite" },
		{3, "Access permission denied" },
		{4, "Callback routine requested an abort" },
		{5, "The database file is locked" },
		{6, "A table in the database is locked" },
		{7, "A malloc() failed" },
		{8, "Attempt to write a readonly database" },
		{9, "Operation terminated by sqlite3_interrupt()" },
		{10, "Some kind of disk I/O error occurred" },
		{11, "The database disk image is malformed" },
		{12, "Unknown opcode in sqlite3_file_control()" },
		{13, "Insertion failed because database is full" },
		{14, "Unable to open the database file" },
		{15, "Database lock protocol error" },
		{16, "Internal use only" },
		{17, "The database schema changed" },
		{18, "String or BLOB exceeds size limit" },
		{19, "Abort due to constraint violation" },
		{20, "Data type mismatch" },
		{21, "Library used incorrectly" },
		{22, "Uses OS features not supported on host" },
		{23, "Authorization denied" },
		{24, "Not used" },
		{25, "2nd parameter to sqlite3_bind out of range" },
		{26, "File opened that is not a database file" },
		{27, "Notifications from sqlite3_log()" },
		{28, "Warnings from sqlite3_log()" },
		{100, "sqlite3_step() has another row ready" },
		{101, "sqlite3_step() has finished executing" }
	};


	SqliteDataManager::SqliteDataManager() 
	{
	}

	SqliteDataManager::~SqliteDataManager()
	{
		dbHandler.Shutdown();
	}


	bool SqliteDataManager::VacuumDb()
	{
		//TODO: this allows to reduce the size of the db once in a while.
		return false;
	}

	bool SqliteDataManager::Encrypt(const std::string & salt)
	{
		// TODO: encryption of db using sqlite's method.
		return true;
	}

	bool SqliteDataManager::Authenticate(const std::string & userId, const std::string & pwd)
	{
		//TODO: Authentication mechanism using sqlite method.
		return true;
	}

	std::string SqliteDataManager::ConditionsAsWhereClause(const string& tablename, const CONDITIONS& cont, const string& combiner)
	{
		CONDITIONS fieldtypes;
		int ret = ExecSqlNative(
			"pragma table_info(" + tablename + ")", 
			[&fieldtypes](void* p, int argc, char** argv, char** azColName) -> int
		{
			string fieldName;
			string fieldValue;
			for (int i = 0; i < argc; i++)
			{
				if (i == 1)
				{
					fieldName = argv[i];
				}
				if (i == 2)
				{
					fieldValue = argv[i];
				}
				if (fieldName.size() && fieldValue.size())
				{
					fieldtypes.insert(make_pair(fieldName, fieldValue));
					break;
				}
			}
			return SQLITE_OK;
		}
		);
		if (IsSqlOk(ret))
		{
			ostringstream ostr;
			auto sz = cont.size();
			for (auto kvp : cont)
			{
				auto fldType = fieldtypes.at(kvp.first);
				if ((std::strstr(fldType.c_str(), "CHAR")) || (fldType == "TEXT"))
				{
					ostr << kvp.first << "=" << "'" << kvp.second << "' ";
				}
				else 
				{
					ostr << kvp.first << "=" << kvp.second;
				}
				if (sz < cont.size() - 1)
				{
					ostr << " " << combiner << " ";
				}
				sz++;
			}
			return ostr.str();
		}
		return "";
	}

	void SqliteDataManager::SubmitData(JsonObject ^ jobj)
	{
		String^ strId = jobj->GetNamedString("_id", L"");
		if (!strId->IsEmpty())
		{

			bool isDeleted = false;

			OutputDebugString(jobj ? jobj->ToString()->Data() : L"null object");
			OutputDebugString(L"\n");

			String^ type = jobj->GetNamedString("type", L"");
			if (type->IsEmpty())
			{
				return;
			}

			DataState state = DataState::Unmodified;
			if (type == "appsettings" && currentUser != nullptr)
			{
				String^ id = jobj->GetNamedString("_id", nullptr);
				if (id != nullptr && id == currentUser->Name)
				{
					UpdateAppSettings(jobj);
				}
			}
			else if (type == APPBUSINESSORG)
			{
				state = UpdateAppBusinessOrg(jobj);
			}
			else if (type == PRODUCTCATEGORIES)
			{
				//UpdateCategory(object, isDeleted);
				state = UpdateCategory(jobj, isDeleted);
			}
			else if (type == PRODUCTCATEGORYCATEGORYREL)
			{
				state = UpdateCategoryToCategoryRelations(jobj, isDeleted);
			}
			else if (type == PRODUCTFAMILIES)
			{
				state = UpdateProductFamily(jobj, isDeleted);
			}
			else if (type == PRODUCTTABLE)
			{
				state = UpdateProduct(jobj, isDeleted);
			}
			else if (type == CATEGORYTOPRODUCT)
			{
				state = UpdateCategoryToProduct(jobj, isDeleted);
			}
			else if (type == PRODUCTMEDIA)
			{
				state = UpdateProductImage(jobj, isDeleted);
			}
			else if (type == PRODUCTPRICEPLANS)
			{
				state = UpdateProductPricePlan(jobj, isDeleted);
			}
			else if (type == PRODUCTPRICESTDDETAILS)
			{
				state = UpdateStandardPricePlanDetails(jobj, isDeleted);
			}
			else if (type == PRODUCTPRICINGTIERDETAILS)
			{
				state = UpdateTieredPricePlanDetails(jobj, isDeleted);
			}
			else if (type == PRODUCTATTRIBUTE)
			{
				state = UpdateProductAttribute(jobj, isDeleted);
			}
			else if (type == PRODUCTATTRIBUTEVALUE)
			{
				state = UpdateProductAttributeValue(jobj, isDeleted);
			}
			else if (type == CUSTOMERORGS)
			{
				//TODO How to handle LOGO attachments.
				state = UpdateCustomerOrg(jobj, isDeleted);
			}
			else if (type == CORGCOSTCENTER)
			{
				state = UpdateCOrgCostCenter(jobj, isDeleted);
			}
			else if (type == CUSTOMERORGCONTACTS)
			{
				state = UpdateCustomerOrgContact(jobj, isDeleted);
			}
			else if (type == FIELDREP)
			{
				state = UpdateFieldReps(jobj, isDeleted);
			}

			else if (type == SALESORDERS)
			{
				UpdateSalesOrder(jobj);
			}
			else if (type == SALESORDERDETAILS)
			{
				UpdateSalesOrderDetails(jobj);
			}
			else if (type == SALESORDERSTATUSHISTORY)
			{
				UpdateSalesOrderStatusHistory(jobj);
			}
			else if (type == SALESORDERITEMSTATUSHISTORY)
			{
				UpdateSalesOrderStatusItemHistory(jobj);
			}
			else if (type == SALESORDERPAYMENTHISTORY)
			{
				UpdateSalesOrderPaymentHistory(jobj);
			}
			else if (type == INVENTORYLOCATION)
			{
				UpdateInventoryLocation(jobj);
			}
			else if (type == ACTIVEINVENTORY)
			{
				UpdateActiveInventory(jobj);
			}
			else if (type == INVENTORYPROPERTY)
			{
				UpdateInventoryProperty(jobj);
			}
			else if (type == INVENTORYTRX)
			{
				UpdateInventoryTrx(jobj);
			}
			else if (type == ADDRESSES)
			{
				state = UpdateAddress(jobj, isDeleted);
			}
			else if (type == ORDERSTATUS || type == PAYMENTSTATUS || type == PAYMENTINSTRUMENT || type == INVENTORYMOVEMENTTYPE)
			{
				state = UpdateEnumTable(type, jobj);
			}

			if (state == DataState::Error)
			{
				RollbackTrans();

#ifdef _DEBUG
				throw std::runtime_error("Failed to update local cache during data sync.");
#else
				LogException("Failed to update local cache during data sync.");

				return;
#endif
			}
		}
	}

	void SqliteDataManager::SubmitData(JsonArray^ rows)
	{
		/* Users and Roles are ignore for now since it will be used by
			administration apps only. Data downloaded (products, orders, etc)
			are already filtered by user.
		if (type == L"userrole")
		{
			UpdateUserRole(object, isDeleted);
		}
		else if (type == L"user")
		{
			UpdateUser(object, isDeleted);
		}
		else */

		BeginTrans();

		for (auto jval : rows)
		{
			auto jobj = jval->GetObject();
			if (jobj->ValueType != JsonValueType::Object)
			{
				continue;
			}
			
			//if (jobj->HasKey("dummydata"))
			//	continue;

			IJsonValue^ doc;
			if ((doc = jobj->Lookup("doc")) && doc->ValueType == JsonValueType::Object)
			{
				jobj = doc->GetObject();
				if (!jobj)
				{
					continue;
				}

				SubmitData(jobj);
			}
		}
		CommitTrans();
	}

	void SqliteDataManager::PurgeDeletedItems(const std::unordered_set<String^>& deleted)
	{
		/* We only the primary keys (_id) of the documents, we don't know the document type. 
		And that means, we have to scan every table. */
		// for each item generate a delete sql that touches every table in the cache db.
		if (!deleted.size()) return;

		string sql = "select name from sqlite_master where type = 'table' and name not in " + excludedTables;
		String^ deletedItems = StringHelpers::JoinContainer(deleted, L',');
		auto ret = ExecSqlNative(sql, [this, deletedItems](void* p, int argc, char** argv, char** azColName)
		{
			string tblName = argv[0];
			ostringstream osql;
			osql << "delete from " << tblName << " where _id in ('" << STRCONV(deletedItems) << "');";
			ExecSqlNative(osql.str());
			int changes;
			if ((changes = LastSqlChanges()) > 0) // means deleted something
			{
#ifdef _DEBUG
				osql.str("");
				osql << "Deleted " << changes << " rows.";
				OutputDebugStringA("\n");
				OutputDebugStringA(osql.str().c_str());
				OutputDebugStringA("\n");
#endif
				// according to documentation returning a non-zero value will stop executing for every record returned by the sql statement.
				return SQLITE_DELETE;
			}
			return SQLITE_OK;
		});
	}

	int SqliteDataManager::UpdateRowRevisionNumber(const string & id, const string & rev, const string & table)
	{
		ostringstream osql;
		if (table == APPSETTINGS)
		{
			osql << "update " << APPSETTINGS << " set propvalue = '" << rev << "' where propname = '_rev';";
		}
		else if (string::npos == excludedTables.find(table))
		{
			osql << "update " << table << " set _rev = '" << rev << "' where _id = '" << id << "';";
		}
		return ExecSqlNative(osql.str());
	}

	bool SqliteDataManager::ClearDb()
	{

		ostringstream osql;
		ostringstream osqlfind;

		osqlfind << "select name, type from sqlite_master where type = 'table';";
		//osqlfind << "select name, type from sqlite_master where type = 'index';";

		ExecSqlNative(osqlfind.str(), [&osql](void* p, int argc, char** argv, char** azColName)
		{
			osql << "drop " << argv[1] << " " << argv[0] << ";";
			return SQLITE_OK;
		});

		if (!IsSqlOk(ExecSql(osql.str(), nullptr)))
		{
			ThrowException(MessageIds::DBClearingFailed);
		}

		return true;
	}

	bool SqliteDataManager::DropTable(const std::string & tableName)
	{
		string dropTable = "DROP TABLE IF EXISTS " + tableName;
		return ExecSql(dropTable, nullptr) == SQLITE_OK;
	}


	bool SqliteDataManager::CreateDataSchema()
	{
		if (CreateSystemTables() && CreateCommonTables())
		{
			if (CreateProductRelatedTables() && CreateInventoryTables())
			{
				if (CreateCustomerRelatedTables())
				{
					if (CreateSalesOrderRelatedTable())
					{
						return true;
					}
				}
			}
		}
		throw Exception::CreateException(E_FAIL, "Failed to create data schema");
	}

	string SqliteDataManager::GetLastRevisionNumber(const string & id)
	{
		auto url = HttpHelpers::GetCombinedUri(String::Concat(String::Concat(GETDOCURL, "/"), StringHelpers::ConvertToString(id.c_str())));
		cancellation_token_source source;
		cancellation_token tok = source.get_token();
		auto response = HttpHelpers::GetHttpResponse(url, tok);
		OutputDebugStringA("\nGetLastRevisionNumber("); OutputDebugStringA(id.c_str()); OutputDebugStringA(") returned \n");
		OutputDebugString(response->Data()); OutputDebugStringA("\n");
		JsonObject^ updatedObj;
		if (JsonObject::TryParse(response, &updatedObj))
		{
			return STRCONV(updatedObj->GetNamedString("_rev", ""));
		}
		return "";
	}

	bool SqliteDataManager::CreateSystemTables()
	{
		ostringstream osql;
		//osql << "CREATE TABLE IF NOT EXISTS " << DOCUMENTKEYS;
		//osql << " ( tablename TEXT CONSTRAINT DocKeysTableNameUnique UNIQUE ON CONFLICT FAIL, usercode CHAR(5), lastkey INTEGER, UNIQUE(tablename, usercode) ); ";
		osql << "CREATE TABLE IF NOT EXISTS " << APPSETTINGS << " ( propname TEXT PRIMARY KEY, propvalue TEXT NOT NULL, isroaming INTEGER NOT NULL DEFAULT 0, state INTEGER NOT NULL DEFAULT 0);";
		//osql << "CREATE TABLE IF NOT EXISTS " << USERRIGHTS << " ( rights NUMBER NOT NULL CONSTRAINT RightsAreUnique UNIQUE ON CONFLICT IGNORE ); ";
		return IsSqlOk(ExecSql(osql.str()));
	}

	bool SqliteDataManager::CreateCommonTables()
	{
		ostringstream osql;

		// Note: APPBUSINESSORG table simply contains the json stored in objdata field.
		osql << "CREATE TABLE IF NOT EXISTS " << APPBUSINESSORG
			<< " ( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, objdata TEXT NOT NULL ); ";

		osql << "CREATE TABLE IF NOT EXISTS " << ADDRESSES;
		osql << " ( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, line1 TEXT NOT NULL, line2 TEXT, city TEXT NOT NULL, province TEXT NOT NULL, country TEXT NOT NULL, postalcode TEXT); ";
		osql << "CREATE INDEX IF NOT EXISTS " << ADDRESSES << "_cityprovincecountryindex ON " << ADDRESSES << "(city, province, country); ";
		
		// order status list
		osql << "CREATE TABLE IF NOT EXISTS " << ORDERSTATUS << "(osid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT OrderStatusNameIsUnique UNIQUE ON CONFLICT FAIL, "
			<< "publicname TEXT NOT NULL, desc TEXT);";

		// payment status
		osql << "CREATE TABLE IF NOT EXISTS " << PAYMENTSTATUS << "(psid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT PaymentStatusNameIsUnique UNIQUE ON CONFLICT FAIL, "
			<< "publicname TEXT NOT NULL, desc TEXT);";

		// payment method
		osql << "CREATE TABLE IF NOT EXISTS " << PAYMENTINSTRUMENT << "(pmid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT PaymentMethodNameIsUnique UNIQUE ON CONFLICT FAIL, "
			<< "publicname TEXT NOT NULL, desc TEXT);";

		// inventory movements
		osql << "CREATE TABLE IF NOT EXISTS " << INVENTORYMOVEMENTTYPE << "(invmvid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT InventoryMoveTypeIsUnique UNIQUE ON CONFLICT FAIL, "
			<< "publicname TEXT NOT NULL, desc TEXT);";

		auto ret = ExecSql(osql.str());
		if (IsSqlOk(ret))
		{
			ret = PrepopulateMasterTableData();
		}

		return IsSqlOk(ret);
	}

	bool SqliteDataManager::CreateProductRelatedTables()
	{
		ostringstream osql;
		// first create customer table.
		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTCATEGORIES;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, name TEXT, desc TEXT ); ";
		// insert default root node
		osql << "INSERT OR IGNORE INTO " << PRODUCTCATEGORIES << " (_id, state, name, desc) VALUES ( 'rootcategory', 1, 'rootcategory', 'rootcategory') ; ";

		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTCATEGORYCATEGORYREL;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, cid CHAR(32) NOT NULL, pcid CHAR(32) NOT NULL, UNIQUE(cid, pcid, state)); ";

		// then product table.
		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTFAMILIES;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, ";
		osql << "name TEXT NOT NULL, desc TEXT, shortdesc TEXT, notes TEXT, pdefattrid CHAR(23) );";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTFAMILIES << "_nameindex ON " << PRODUCTFAMILIES << " ( name );";

		// then product table.
		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTTABLE;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, skun TEXT  NOT NULL CONSTRAINT ProdSkunIsUnique UNIQUE ON CONFLICT FAIL, ";
		osql << "name TEXT NOT NULL, subname TEXT, desc TEXT, shortdesc TEXT, notes TEXT, pfamilyid CHAR(32), ";
		osql << "priceplanid CHAR(32), pdefattrid CHAR(32), barcode TEXT, isstockless INTEGER, uom TEXT); ";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTTABLE << "_nameindex ON " << PRODUCTTABLE << " ( name );";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTTABLE << "_pfamilyidindex ON " << PRODUCTTABLE << " ( pfamilyid );";

		osql << "CREATE TABLE IF NOT EXISTS " << CATEGORYTOPRODUCT;
		osql << " ( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGEGER NOT NULL DEFAULT 0, cid CHAR(32) NOT NULL, pid CHAR(32) NOT NULL, UNIQUE(cid, pid, state) );";

		// then productattributes table.
		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTATTRIBUTE;
		osql << " ( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, paid CHAR(32) NOT NULL DEFAULT '', name TEXT NOT NULL, desc TEXT, attrtype TEXT );";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTATTRIBUTE << "_parentidindex ON " << PRODUCTATTRIBUTE << " ( paid );";

		// relationship between product and attributes
		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTATTRIBUTEVALUE;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, pid CHAR(32) NOT NULL, aid CHAR(32) NOT NULL, avalue TEXT NOT NULL, avaluetype TEXT NOT NULL, uom TEXT, ";
		osql << "unique(pid, aid, state) );";

		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTPRICEPLANS;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, plancode TEXT CONSTRAINT PlanCodeUnique UNIQUE ON CONFLICT FAIL, plandesc TEXT, dateinit INTEGER NOT NULL, dateended INTEGER NULL );";

		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTPRICESTDDETAILS;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, pppid CHAR(32) NOT NULL, unitprice REAL );";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTPRICESTDDETAILS << "_priceplanindex ON " << PRODUCTPRICESTDDETAILS << " ( pppid );";

		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTPRICINGTIERDETAILS;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, pppid CHAR(32) NOT NULL, unitprice REAL, beginqty INTEGER, endqty INTEGER);";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTPRICINGTIERDETAILS << "_priceplanindex ON " << PRODUCTPRICINGTIERDETAILS << " ( pppid );";

		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTCOMPOSITION;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, pidprimary CHAR(32) NOT NULL, pidsecondary CHAR(32) NOT NULL, relation TEXT, reldesc TEXT, UNIQUE(pidprimary, pidsecondary, state) );";

		osql << "CREATE TABLE IF NOT EXISTS " << PRODUCTMEDIA;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, pid CHAR(32) NOT NULL, istitleimage integer default 0, data text not null, contenttype text not null, title TEXT, desc TEXT);";
		osql << "CREATE INDEX IF NOT EXISTS " << PRODUCTMEDIA << "_pidindex ON " << PRODUCTMEDIA << " ( pid );";

		return IsSqlOk(ExecSql(osql.str()));
	}

	bool SqliteDataManager::CreateInventoryTables()
	{
		ostringstream osql;
		osql << "CREATE TABLE IF NOT EXISTS " << INVENTORYLOCATION
			<< "( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, "
			<< "location TEXT NOT NULL, code TEXT NOT NULL, contactid CHAR(32), addressid CHAR(32), UNIQUE (location, code)); ";
		osql << "CREATE TABLE IF NOT EXISTS " << ACTIVEINVENTORY
			<< "( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, "
			<< "pid CHAR(32) NOT NULL, pqty NUMBER NOT NULL DEFAULT 0, location TEXT, UNIQUE (pid, location) ); ";
		osql << "CREATE TABLE IF NOT EXISTS " << INVENTORYPROPERTY
			<< "( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, "
			<< "pid CHAR(32) NOT NULL CONSTRAINT PidUniqueConstraintInInvProp UNIQUE ON CONFLICT FAIL, "
			<< "minqty NUMBER NOT NULL DEFAULT 0, maxqty NUMBER NOT NULL DEFAULT 0, "
			<< "refilltype NUMBER NOT NULL DEFAULT 0 );";
		osql << "CREATE TABLE IF NOT EXISTS " << INVENTORYTRX
			<< "( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, "
			<< "pid CHAR(32) NOT NULL, idate NUMBER NOT NULL, qtymoved NUMBER NOT NULL, movetype NUMBER NOT NULL, "
			<< "movedir NUMBER NOT NULL, inouttag CHAR(32) ); "
			<< "CREATE INDEX IF NOT EXISTS " << INVENTORYTRX << "_pidAndDateIndex ON " << INVENTORYTRX << " (pid, idate); ";
		return IsSqlOk(ExecSql(osql.str()));
	}

	bool SqliteDataManager::CreateSalesOrderRelatedTable()
	{
		ostringstream osql;
		osql << "CREATE TABLE IF NOT EXISTS " << SALESORDERS << " "
			<< "(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, onumber TEXT CONSTRAINT CatNameUnique UNIQUE ON CONFLICT FAIL, "
			<< "corgccid CHAR(32), batchno TEXT, odate NUMBER, cccontactid CHAR(32), repid TEXT, ototalgross REAL NOT NULL, "
			<< "disccode TEXT, discnum REAL, disctype TEXT, taxpct REAL, taxvalue REAL, currencyid TEXT, ototalfinal REAL NOT NULL, onotes TEXT, customerexpecteddate NUMBER, owner TEXT); ";

		osql << "CREATE TABLE IF NOT EXISTS " << SALESORDERDETAILS << " "
			<< "(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, soid CHAR(32) NOT NULL, "
			<< "pid CHAR(32) NOT NULL, pqty INTEGER NOT NULL DEFAULT 0, unitprice REAL, grossprice REAL, pdisc REAL, pdisctype TEXT, "
			<< "netprice REAL NOT NULL DEFAULT 0, "
			<< "isexpedited INTEGER DEFAULT 0, customerexpecteddate NUMBER, owner TEXT); "
			<< "CREATE INDEX IF NOT EXISTS " << SALESORDERDETAILS << "_soidindex ON " << SALESORDERDETAILS << " ( soid );";

		osql << "CREATE TABLE IF NOT EXISTS " << SALESORDERSTATUSHISTORY << " "
			<< "(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, "
			<< "soid CHAR(32) NOT NULL, ostatus INTEGER NOT NULL, ostatusdate NUMBER NOT NULL, "
			<< "notes TEXT, createdby TEXT NOT NULL, UNIQUE(soid, ostatus, ostatusdate, state) ); ";
		osql << "CREATE INDEX IF NOT EXISTS " << SALESORDERSTATUSHISTORY << "_soidindex ON " << SALESORDERSTATUSHISTORY << " (soid);";

		osql << "CREATE TABLE IF NOT EXISTS " << SALESORDERITEMSTATUSHISTORY << " "
			<< "(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, "
			<< "ositemid CHAR(32) NOT NULL, oslid CHAR(32) NOT NULL, osstatus INTEGER NOT NULL, "
			<< "paymentstatus INTEGER, notes TEXT, createdby TEXT NOT NULL, UNIQUE (ositemid, oslid, state) ); ";

		osql << "CREATE TABLE IF NOT EXISTS " << SALESORDERPAYMENTHISTORY << " "
			<< "(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, soid CHAR(32) NOT NULL, paymentstatus INTEGER NOT NULL, paymentinstrument NOT NULL, "
			<< "paymentdate NUMBER NOT NULL, currency TEXT, paidamount REAL NOT NULL, paydetails TEXT);";
		osql << "CREATE INDEX IF NOT EXISTS " << SALESORDERPAYMENTHISTORY << "_sophistoryindex ON " << SALESORDERPAYMENTHISTORY << " (soid);";

		return IsSqlOk(ExecSql(osql.str()));
	}

	bool SqliteDataManager::CreateCustomerRelatedTables()
	{
		ostringstream osql;
		osql << "CREATE TABLE IF NOT EXISTS " << CUSTOMERORGS;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, ";
		//osql << " ccode TEXT NOT NULL CONSTRAINT CustomerOrgCodeUnique UNIQUE ON CONFLICT FAIL, ";
		osql << " ccode TEXT NOT NULL, ";
		osql << " name TEXT NOT NULL,  url TEXT, facebook TEXT, linkedin TEXT, notes TEXT, isactive INTEGER NOT NULL, logo TEXT); ";

		osql << "CREATE TABLE IF NOT EXISTS " << CORGCOSTCENTER;
		osql << " ( _id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, ";
		osql << "corgid CHAR(32) NOT NULL, name TEXT NOT NULL, code TEXT NOT NULL, paymentmethodcode TEXT, paymenttermcode TEXT, ";
		osql << "invoicedesccode TEXT, creditterm NUMBER, creditlimit NUMBER, shippingagentcode TEXT, ";
		osql << "phone TEXT, email TEXT, fax TEXT, mailaddr CHAR(32), billaddr CHAR(32), shipaddr CHAR(32), ";
		osql << "searchname TEXT, notes TEXT, isactive INTEGER NOT NULL, isdefault INTEGER NOT NULL );";

		osql << "CREATE TABLE IF NOT EXISTS " << CUSTOMERORGCONTACTS;
		osql << " (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, corgccid CHAR(32) NOT NULL, firstname TEXT, lastname TEXT, ";
		osql << "phone1 TEXT, phone2 TEXT, cellphone TEXT, email TEXT, facebook TEXT, linkedin TEXT ); ";

		osql << "CREATE INDEX IF NOT EXISTS " << CUSTOMERORGCONTACTS << "_corgccidindex ON " << CUSTOMERORGCONTACTS << " ( corgccid );";

		osql << "CREATE TABLE IF NOT EXISTS " << FIELDREP;
		osql << "(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, ";
		osql << "uid TEXT, corgccid CHAR(32), scode TEXT, srole TEXT, notes TEXT, UNIQUE(uid, corgccid, state) );";

		return IsSqlOk(ExecSql(osql.str()));
	}

	bool SqliteDataManager::CreatePersonnelRelatedTables()
	{
		return false;
	}

	int SqliteDataManager::PrepopulateMasterTableData()
	{
		ostringstream osql;

		/*
		Order Status Type
		Public Name	Private Name	Seq Number	Notes
		Draft	Draft
		Submitted	Submitted
		Pending	Credit Conditions Not Met
		Pending	Credit Condition Negotiation in progress
		Pending	Inventory Check in progress
		Processing	Await Payment
		Processing	Await Fulfillment
		Processing	Await Shipping
		Declined	Declined
		Cancel Requested	Cancel Requested
		Cancelled	Cancelled 		Order can be cancelled before order is Processed.
		Processed	Processed		Ready for shipment
		Partially Shipped	Partially Shipped
		Shipped	Shipped
		Delivered	Delivered
		Completed	Completed

		*/

		initializer_list<LPCSTR[4]> orderStatuses = 
		{
			{ "0", "Unknown", "Not Available", "Order status currently not available" },
			{ "1", "Draft", "Draft", "Draft Order"},
			{ "2", "Submitted", "Submitted", "Order is submitted" },
			{ "3", "Inventory Check", "Processing", "Order is being processed" },
			{ "4", "Customer Credit Check", "Processing", "Order is being processed" },
			{ "5", "Awaiting Payment", "Processing", "Order is being processed" },
			{ "6", "Payment Received", "Processing", "Order payment is received" },
			{ "7", "Await Fulfillment ", "Processing", "Order is being filled" },
			{ "8", "Processed", "Processed", "Order is processed and is getting ready for shipment" },
			{ "9", "Shipped", "Shipped", "Ordered items are shipped" },
			{ "10", "Delivered", "Delivered", "Order items have been delivered" },
			{ "11", "Completed", "Completed", "Order has been completely processed, shipped and delivered" },
			// other situations
			{ "12", "Cancel Requested", "Cancel Requested", "%s initiated a cancel request" }, //%s is replaced with customer's name
			{ "13", "Cancelled", "Cancelled", "Cancel request is accepted" }, 
			{ "14", "Partially Cancelled", "Part Cancel", "Some items in the order have been canceled due to cancel request" },
			{ "15", "Partially Shipped", "Partially Shipped", "Shipment is being made on a partial basis due to some back ordered items"},
			{ "16", "Payment Declined", "Payment Declined", "Customer payment is declined by payment processor" },
			{ "17", "Partially Shipped", "Partially Shipped", "Shipment is being made on a partial basis" },
			{ "18", "Back Ordered", "Back Ordered", "Order is currently in back order state" },
		};
		//osid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT OrderStatusNameIsUnique ON CONFLICT FAIL, "
		//<< "publicname TEXT NOT NULL, desc TEXT
		for (auto item : orderStatuses)
		{
			osql << "insert or ignore into " << ORDERSTATUS
				<< " (osid, state, privatename, publicname, desc) "
				<< "values "
				<< "( " << item[0] << ", " << (int)DataState::Added << ", '" << item[1] << "', '" << item[2] << "', '" << item[3] << "' );";
		}

		/*
		Payment
		Public Name	Private Name	Seq Number
		Awaited
		Declined
		Received
		Partially Paid
		Fully Paid
		Refunded

		*/

		initializer_list<LPCSTR[4]> paymentStatuses =
		{
			{ "0", "Unknown", "Not Available", "Payment status currently unavailable" },
			{ "1", "Awaited", "Awaited", "Awaiting payment" },
			{ "2", "Declined", "Declined", "Payment is declined by the payment processor" },
			{ "3", "Received", "Received", "Payment received" },
			{ "4", "Partial", "Partial Payment", "Partial payment received" },
			{ "5", "Fully Paid", "Fully Paid", "Full payment received" },
			{ "6", "Refund In Progress", "Refund In Progress", "Order has been cancelled and payment refund is in progress" },
			{ "7", "Refunded", "Refunded", "Order is been canceled and payment is refunded" },
		};

		//(psid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT PaymentStatusNameIsUnique ON CONFLICT FAIL, "
			//<< "publicname TEXT NOT NULL, desc TEXT);		
		for (auto item : paymentStatuses)
		{
			osql << "insert or ignore into " << PAYMENTSTATUS
				<< "(psid, state, privatename, publicname, desc) "
				<< "values "
				<< "( " << item[0] << ", " << (int)DataState::Added << ", '" << item[1] << "', '" << item[2] << "', '" << item[3] << "' );";
		}

		// Paypal 

		initializer_list<LPCSTR[4]> paymentInstruments =
		{
			{ "0", "Unknown", "Not Available", "Payment instrument unavailable" },
			{ "1", "Paypal", "Paypal", "Paypal Online Payment System" },
		};
		//(pmid INTEGER PRIMARY KEY, state INTEGER NOT NULL DEFAULT 0, privatename TEXT NOT NULL CONSTRAINT PaymentMethodNameIsUnique ON CONFLICT FAIL, "
		//<< "publicname TEXT NOT NULL, desc TEXT)
		for (auto item : paymentInstruments)
		{
			osql << "insert or ignore into " << PAYMENTINSTRUMENT
				<< "(pmid, state, privatename, publicname, desc) "
				<< "values "
				<< "( " << item[0] << ", " << (int)DataState::Added << ", '" << item[1] << "', '" << item[2] << "', '" << item[3] << "' );";
		}

		initializer_list<LPCSTR[4]> invMovements =
		{
			{ "0", "Unknown", "Not Available", "Inventory movement type not known" },
			{ "1", "In-Init", "Initial Entry Of Stock Item", "Initial Entry Of Stock Item" },
			{ "2", "In-Renew", "Fresh stock from supplier", "Fresh stock from supplier" },
			{ "3", "RSRVD-For-In-Order", "Reserved to satisfy an order placed", "Reserved to satisfy an order placed" },
			{ "4", "Out-Shipped", "Shipped out to a customer costcenter", "Shipped out to a customer costcenter" },
			{ "5", "In-Ret", "Returned by customer", "Returned by customer" },
			{ "6", "MV-Loc-To-Loc", "Moved from one inventory location to another", "Moved from one inventory location to another" },
			{ "7", "Split", "Split one product to multiple components", "Split one product to multiple components" },
			{ "8", "Combined", "Combined multiple product units into one unit", "Combined multiple product units into one unit" },
			{ "9", "Correction", "Inventory Correction", "Inventory Correction" },  // experimental - reason must be provided
		};

		for (auto item : invMovements)
		{
			osql << "insert or ignore into " << INVENTORYMOVEMENTTYPE
				<< "(invmvid, state, privatename, publicname, desc) "
				<< "values "
				<< "( " << item[0] << ", " << (int)DataState::Added << ", '" << item[1] << "', '" << item[2] << "', '" << item[3] << "' );";
		}

		return ExecSqlNative(osql.str());
	}

	int SqliteDataManager::DeleteLocalItems(std::initializer_list<string> tables)
	{
		ostringstream osqltables;
		for(auto& tbl : tables)
			osqltables << "delete from " << tbl << " where state = " << (int)DataState::Deleted << ";";
		return ExecSqlNative(osqltables.str());
	}

	JsonObject ^ SqliteDataManager::GetRemoteObject(String^ documentId)
	{
		auto uri = HttpHelpers::GetCombinedUri(String::Concat(String::Concat(GETDOCURL, "/"), documentId));
		cancellation_token_source source;
		cancellation_token tok = source.get_token();
		auto response = HttpHelpers::GetHttpResponse(uri, tok);
		JsonObject^ updatedObj;
		if (JsonObject::TryParse(response, &updatedObj))
		{
			return updatedObj;
		}
		return nullptr;
	}

	void SqliteDataManager::GetCommonDataLocalChanges(JsonArray ^ fill)
	{

		int ret = 0;
		ostringstream osql;
		string rev;
		JsonObject^ obj;
		JsonArray^ arr;

		// get appsettings object in the server to compare with.
		auto serverObj = GetRemoteObject(currentUser->Name);
		// app settings
		// fetch all roaming items from appsettings table.
		osql << "select propname, propvalue, state from appsettings where isroaming = 1;";
		obj = ref new JsonObject();
		String^ revNum = serverObj != nullptr ? serverObj->GetNamedString("_rev", "") : "";
		arr = serverObj != nullptr ? serverObj->GetNamedArray("props", ref new JsonArray()) : ref new JsonArray();
		auto arrSize = arr->Size;
		ret = ExecSqlNative(osql.str(), [this, &arr, fill](void* p, int argc, char** argv, char** azColName)
		{
			if (!strcmp(argv[0], "_rev"))
			{
				//obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			}
			else
			{
				auto state = (DataState)std::stoi(argv[2]);
				if (state == DataState::Added || state == DataState::Edited)
				{
					String^ propName = STRCONV(argv[0]);
					if (!StringHelpers::IsNullOrEmpty(propName))
					{
						auto v = to_vector(arr);
						auto pos = std::find_if(v.cbegin(), v.cend(), [propName](IJsonValue^ itm)
						{
							JsonObject^ obj = itm->GetObject();
							auto pname = obj->GetNamedString("propname");
							return propName == pname;
						});
						JsonObject^ arrmem = ref new JsonObject();
						arrmem->SetNamedValue("propname", JsonValue::CreateStringValue(propName));
						arrmem->SetNamedValue("propval", JsonValue::CreateStringValue(STRCONV(argv[1])));
						if (pos == v.cend())
						{
							arr->Append(arrmem);
						}
						else
						{
							arr->SetAt(pos - v.cbegin(), arrmem);
						}
					}
				}
			}
			return SQLITE_OK;
		});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		if (arr->Size > 0)
		{
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(currentUser->Name));
			if (!StringHelpers::IsNullOrEmpty(revNum))
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(revNum));
			obj->SetNamedValue("type", JsonValue::CreateStringValue("appsettings"));
			obj->SetNamedValue("props", arr);
			fill->Append(obj);
		}

		arr = nullptr;
		obj = nullptr;

		// order status
		/* 
		json notation
		{
			_id = "orderstatus",
			type = "orderstatus",
			"items" : [
				{
					"osid" : 1, 
					"privatename = "abc",
					"publicname" = "xyz",
					"desc" = "abc xyz"
				},.....
			]
		*/
		// fetch revision number first so uploading will be successful.
		rev = GetLastRevisionNumber(ORDERSTATUS);
		osql.str("");
		//   0      1         2           3       4
		//(osid, state, privatename, publicname, desc)
		// NOTE: delete is not included .
		osql << "select osid, state, privatename, publicname, desc from " << ORDERSTATUS << " where state in (1, 2) order by osid;";
		arr = ref new JsonArray();
		ret = ExecSqlNative(osql.str(), [arr](void* p, int argc, char** argv, char** azColName)
		{
			JsonObject^ j = ref new JsonObject();
			j->SetNamedValue("osid", JsonValue::CreateNumberValue(strtol(argv[0], nullptr, 10)));
			j->SetNamedValue("privatename", JsonValue::CreateStringValue(STRCONV(argv[2])));
			j->SetNamedValue("publicname", JsonValue::CreateStringValue(STRCONV(argv[3])));
			j->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[4])));
			arr->Append(j);
			return SQLITE_OK;
		});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		if (arr->Size > 0)
		{
			obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(ORDERSTATUS));
			if (!rev.empty())
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(StringHelpers::ConvertToString(rev.c_str())));
			obj->SetNamedValue("type", JsonValue::CreateStringValue(ORDERSTATUS));
			obj->SetNamedValue("items", arr);
			fill->Append(obj);
			arr = nullptr;
			obj = nullptr;
		}

		// payment status
		rev = GetLastRevisionNumber(PAYMENTSTATUS);
		osql.str("");
		//   0      1         2           3       4
		//(psid, state, privatename, publicname, desc)
		// NOTE: delete is not included .
		osql << "select psid, state, privatename, publicname, desc from " << PAYMENTSTATUS << " where state in (1, 2) order by psid;";
		arr = ref new JsonArray();
		ret = ExecSqlNative(osql.str(), [arr](void* p, int argc, char** argv, char** azColName)
		{
			JsonObject^ j = ref new JsonObject();
			j->SetNamedValue("psid", JsonValue::CreateNumberValue(strtol(argv[0], nullptr, 10)));
			j->SetNamedValue("privatename", JsonValue::CreateStringValue(STRCONV(argv[2])));
			j->SetNamedValue("publicname", JsonValue::CreateStringValue(STRCONV(argv[3])));
			j->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[4])));
			arr->Append(j);
			return SQLITE_OK;
		});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		if (arr->Size > 0)
		{
			obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(PAYMENTSTATUS));
			if (!rev.empty())
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(StringHelpers::ConvertToString(rev.c_str())));
			obj->SetNamedValue("type", JsonValue::CreateStringValue(PAYMENTSTATUS));
			obj->SetNamedValue("items", arr);
			fill->Append(obj);
			arr = nullptr;
			obj = nullptr;
		}

		// payment instruments
		rev = GetLastRevisionNumber(PAYMENTINSTRUMENT);
		osql.str("");
		//   0      1         2           3       4
		//(psid, state, privatename, publicname, desc)
		// NOTE: delete is not included .
		osql << "select pmid, state, privatename, publicname, desc from " << PAYMENTINSTRUMENT << " where state in (1, 2) order by pmid;";
		arr = ref new JsonArray();
		ret = ExecSqlNative(osql.str(), [arr](void* p, int argc, char** argv, char** azColName)
		{
			JsonObject^ j = ref new JsonObject();
			j->SetNamedValue("psid", JsonValue::CreateNumberValue(strtol(argv[0], nullptr, 10)));
			j->SetNamedValue("privatename", JsonValue::CreateStringValue(STRCONV(argv[2])));
			j->SetNamedValue("publicname", JsonValue::CreateStringValue(STRCONV(argv[3])));
			j->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[4])));
			arr->Append(j);
			return SQLITE_OK;
		});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		if (arr->Size > 0)
		{
			obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(PAYMENTINSTRUMENT));
			if (!rev.empty())
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(StringHelpers::ConvertToString(rev.c_str())));
			obj->SetNamedValue("type", JsonValue::CreateStringValue(PAYMENTINSTRUMENT));
			obj->SetNamedValue("items", arr);
			fill->Append(obj);
			arr = nullptr;
			obj = nullptr;
		}

		// inventory movement types
		rev = GetLastRevisionNumber(INVENTORYMOVEMENTTYPE);
		osql.str("");
		//   0      1         2           3       4
		//(psid, state, privatename, publicname, desc)
		// NOTE: delete is not included .
		osql << "select invmvid, state, privatename, publicname, desc from " << INVENTORYMOVEMENTTYPE << " where state in (1, 2) order by invmvid;";
		arr = ref new JsonArray();
		ret = ExecSqlNative(osql.str(), [arr](void* p, int argc, char** argv, char** azColName)
		{
			JsonObject^ j = ref new JsonObject();
			j->SetNamedValue("invmvid", JsonValue::CreateNumberValue(strtol(argv[0], nullptr, 10)));
			j->SetNamedValue("privatename", JsonValue::CreateStringValue(STRCONV(argv[2])));
			j->SetNamedValue("publicname", JsonValue::CreateStringValue(STRCONV(argv[3])));
			j->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[4])));
			arr->Append(j);
			return SQLITE_OK;
		});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		if (arr->Size > 0)
		{
			obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(INVENTORYMOVEMENTTYPE));
			if (!rev.empty())
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(StringHelpers::ConvertToString(rev.c_str())));
			obj->SetNamedValue("type", JsonValue::CreateStringValue(INVENTORYMOVEMENTTYPE));
			obj->SetNamedValue("items", arr);
			fill->Append(obj);
			arr = nullptr;
			obj = nullptr;
		}


		// address
		osql.str("");
		//                0      1   2      3     4      5       6         7          8
		osql << "select _id, _rev, state, line1, line2, city, province, postalcode, country from " << ADDRESSES << " where state in (1,2,3)";

		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("line1", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("line2", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("city", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("province", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("postalcode", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("country", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(ADDRESSES)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, objdata from " << APPBUSINESSORG << " where state in (1,2,3)";
		ret = DBSELECTOR(osql.str())
		{
			auto state = (DataState)std::stoi(argv[2]);
			// construct json from objdata and update id, rev
			JsonObject^ obj = nullptr;
			if (JsonObject::TryParse(STRCONV(argv[3]), &obj))
			{
				obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
				if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
					obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(APPBUSINESSORG)));
				fill->Append(obj);
			}
			return SQLITE_OK;
		});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		return;

	ONERROR:
		ThrowException(MessageIds::DBQueryFatalError);

	}

	void SqliteDataManager::GetProdCategoryLocalChanges(JsonArray^ fill)
	{
		// query all records with state != 0
		ostringstream osql;
		osql << "select _id, _rev, name, desc, state from " << PRODUCTCATEGORIES << " where state in (1,2,3)";
		int ret = 0;
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			if (argc == 5)
			{
				auto state = (DataState)std::stoi(argv[4]);
				JsonObject ^ obj = ref new JsonObject();
				obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
				if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
					obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
				if (state == DataState::Deleted)
					obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
				else
				{
					obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[2])));
					obj->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[3])));
					obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTCATEGORIES)));
				}
				fill->Append(obj);
				return SQLITE_OK;
			}
			else
			{
				throw CreateException(MessageIds::DBQueryFatalError);
			}
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		// bulk update relations
		osql.str("");
		osql << "select _id, _rev, cid, pcid, state from " << PRODUCTCATEGORYCATEGORYREL << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			if (argc == 5)
			{
				auto state = (DataState)std::stoi(argv[4]);
				JsonObject ^ obj = ref new JsonObject();
				obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
				if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
					obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
				if (state == DataState::Deleted)
					obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
				else {
					obj->SetNamedValue("cid", JsonValue::CreateStringValue(STRCONV(argv[2])));
					obj->SetNamedValue("pcid", JsonValue::CreateStringValue(STRCONV(argv[3])));
					obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTCATEGORYCATEGORYREL)));
				}
				fill->Append(obj);
				return SQLITE_OK;
			}
			else
			{
				throw CreateException(MessageIds::DBQueryFatalError);
			}
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		//ret = DeleteLocalItems({ PRODUCTCATEGORIES, PRODUCTCATEGORYCATEGORYREL });

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		return;

	ONERROR:
		ThrowException(MessageIds::DBQueryFatalError);
	}

	void SqliteDataManager::GetProductLocalChanges(JsonArray ^ fill)
	{
		/* Fill products, categorytoproductfamily, productattributelist, productpricingplans,
		productpricetierdetails, productcomposition, productattributes, productmedia if any changes
		exists in local db */

		// FETCH ALL CHANGES where state > DataState::Unmodified.
		ostringstream osql;
		int ret = 0;
		//osql << "select _id, _rev, name, desc, shortdesc, notes, state from " << PRODUCTFAMILIES << " where state in (1,2,3)";
		//int ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		//{
		//	auto state = (DataState)std::stoi(argv[6]);
		//	JsonObject ^ obj = ref new JsonObject();
		//	obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
		//	if (state == DataState::Edited && argv[1])
		//		obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
		//	obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[2])));
		//	obj->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[3])));
		//	obj->SetNamedValue("shortdesc", JsonValue::CreateStringValue(STRCONV(argv[4])));
		//	obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[5])));
		//	obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTFAMILIES)));
		//	if (state == DataState::Deleted)
		//		obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
		//	fill->Append(obj);
		//	return SQLITE_OK;
		//}, nullptr);
		//if (!IsSqlOk(ret))
		//{
		//	goto ONERROR;
		//}
		////TODO -> Products, CategoryTOProducts ProductsInFamily, ProductAttributes, ProductMedia, etc.
		//osql.str("");
		osql << "select _id, _rev, name, subname, desc, shortdesc, notes, pfamilyid, pdefattrid, priceplanid, isstockless, uom, skun, barcode, state from " << PRODUCTTABLE << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[14]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[2])));
				obj->SetNamedValue("subname", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("shortdesc", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("pfamilyid", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("pdefattrid", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("priceplanid", JsonValue::CreateStringValue(STRCONV(argv[9])));
				obj->SetNamedValue("isstockless", JsonValue::CreateBooleanValue((STRCONV(argv[10]) == "1" ? true : false)));
				obj->SetNamedValue("uom", JsonValue::CreateStringValue(STRCONV(argv[11])));
				obj->SetNamedValue("skun", JsonValue::CreateStringValue(STRCONV(argv[12])));
				obj->SetNamedValue("barcode", JsonValue::CreateStringValue(STRCONV(argv[13])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTTABLE)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, cid, pid from " << CATEGORYTOPRODUCT << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("cid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(CATEGORYTOPRODUCT)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		//TODO -> Products, ProductsInFamily, ProductAttributes, ProductMedia, etc.
		osql.str("");
		osql << "select _id, _rev, state, pid, istitleimage, data, contenttype, title, desc from " << PRODUCTMEDIA << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("istitleimage", JsonValue::CreateBooleanValue((STRCONV(argv[4]) == "1" ? true : false)));
				obj->SetNamedValue("data", JsonValue::CreateStringValue(STRCONV(argv[5]))); //TODO BLOB real data conversion
				obj->SetNamedValue("contenttype", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("title", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTMEDIA)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, plancode, plandesc, dateinit, dateended from " << PRODUCTPRICEPLANS 
			<< " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("plancode", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("plandesc", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("dateinit", JsonValue::CreateStringValue(STRCONV(argv[5])));
				if (argv[6])
					obj->SetNamedValue("dateended", JsonValue::CreateStringValue(STRCONV(argv[6])));
				else
					obj->SetNamedValue("dateended", JsonValue::CreateNullValue());
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTPRICEPLANS)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, pppid, unitprice from " << PRODUCTPRICESTDDETAILS
			<< " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pppid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("unitprice", JsonValue::CreateNumberValue(atof(argv[4])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTPRICESTDDETAILS)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, pppid, unitprice, beginqty, endqty from " << PRODUCTPRICINGTIERDETAILS
			<< " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pppid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("unitprice", JsonValue::CreateNumberValue(atof(argv[4])));
				obj->SetNamedValue("beginqty", JsonValue::CreateNumberValue(atof(argv[5])));
				obj->SetNamedValue("endqty", JsonValue::CreateNumberValue(atof(argv[6])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTPRICINGTIERDETAILS)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		//                0    1    2      3    4       5         6         7
		osql << "select _id, _rev, state, name, desc, shortdesc, notes, pdefattrid from " << PRODUCTFAMILIES << " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("shortdesc", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("pdefattrid", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTFAMILIES)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, paid, name, desc, attrtype from " << PRODUCTATTRIBUTE << " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("paid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("desc", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("attrtype", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTATTRIBUTE)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, pid, aid, avalue, avaluetype, uom from " << PRODUCTATTRIBUTEVALUE << " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("aid", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("avalue", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("avaluetype", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("uom", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(PRODUCTATTRIBUTEVALUE)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		//ret = DeleteLocalItems(
		//{ 
		//	PRODUCTFAMILIES, 
		//	PRODUCTTABLE, 
		//	CATEGORYTOPRODUCT, 
		//	PRODUCTMEDIA, 
		//	PRODUCTPRICEPLANS, 
		//	PRODUCTPRICESTDDETAILS, 
		//	PRODUCTPRICINGTIERDETAILS,
		//	PRODUCTATTRIBUTE,
		//	PRODUCTATTRIBUTEVALUE
		//});

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		return;

		ONERROR:
			ThrowException(MessageIds::DBQueryFatalError);
	}

	void SqliteDataManager::GetInventoryLocalChanges(JsonArray ^ fill)
	{
		ostringstream osql;
		//               0     1    2        3      4       5			6
		osql << "select _id, _rev, state, location, code, contactid, addressid FROM " << INVENTORYLOCATION
			<< " where state in (1, 2, 3);";
		int ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("location", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("code", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("contactid", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("addressid", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(INVENTORYLOCATION)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);

		//                0    1    2     3      4       5
		osql << "select _id, _rev, state, pid, pqty, location FROM " << ACTIVEINVENTORY
			<< " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("pqty", JsonValue::CreateNumberValue(std::strtol(argv[4], nullptr, 10)));
				obj->SetNamedValue("location", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(ACTIVEINVENTORY)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		osql.str("");
		//               0      1   2      3     4      5          6
		osql << "select _id, _rev, state, pid, minqty, maxqty, refilltype FROM " << INVENTORYPROPERTY
			<< " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("minqty", JsonValue::CreateNumberValue(std::strtol(argv[4], nullptr, 10)));
				obj->SetNamedValue("maxqty", JsonValue::CreateNumberValue(std::strtol(argv[5], nullptr, 10)));
				obj->SetNamedValue("refilltype", JsonValue::CreateNumberValue(std::strtol(argv[6], nullptr, 10)));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(INVENTORYPROPERTY)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		osql.str("");
		//               0      1   2      3     4      5          6        7       8  
		osql << "select _id, _rev, state, pid, idate, qtymoved, movetype, movedir, inouttag FROM " << INVENTORYTRX
			<< " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("idate", JsonValue::CreateNumberValue(strtol(argv[4], nullptr, 10)));
				obj->SetNamedValue("qtymoved", JsonValue::CreateNumberValue(std::strtol(argv[5], nullptr, 10)));
				obj->SetNamedValue("movetype", JsonValue::CreateNumberValue(std::strtol(argv[6], nullptr, 10)));
				obj->SetNamedValue("movedir", JsonValue::CreateNumberValue(std::strtol(argv[7], nullptr, 10)));
				obj->SetNamedValue("inouttag", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(INVENTORYTRX)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		osql.str("");

		return;

	ONERROR:
		ThrowException(MessageIds::DBQueryFatalError);

	}

	void SqliteDataManager::GetOrdersLocalChanges(JsonArray ^ fill)
	{
		/* productinventory table + all sales order related tables if any local changes exists. */
		ostringstream osql;
		//               0     1      2      3       4           5    6       7             8         9         
		osql << "select _id, _rev, state, onumber, corgccid, batchno, odate, cccontactid, repid, ototalgross, "
			//    10       11        12        13         14        15       16           17              18           19
			<< "disccode, discnum, disctype, taxpct, taxvalue, currencyid, ototalfinal, onotes, customerexpecteddate, owner "
			<< "from " << SALESORDERS << " where state in (1,2,3);";
		int ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("onumber", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("corgccid", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("batchno", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("odate", JsonValue::CreateNumberValue(strtol(argv[6], nullptr, 10)));
				obj->SetNamedValue("cccontactid", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("repid", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("ototalgross", JsonValue::CreateNumberValue(std::strtod(argv[9], nullptr)));
				obj->SetNamedValue("disccode", JsonValue::CreateStringValue(STRCONV(argv[10])));
				obj->SetNamedValue("discnum", JsonValue::CreateNumberValue(std::strtod(argv[11], nullptr)));
				obj->SetNamedValue("disctype", JsonValue::CreateStringValue(STRCONV(argv[12])));
				obj->SetNamedValue("taxpct", JsonValue::CreateNumberValue(std::strtod(argv[13], nullptr)));
				obj->SetNamedValue("taxvalue", JsonValue::CreateNumberValue(std::strtod(argv[14], nullptr)));
				obj->SetNamedValue("currencyid", JsonValue::CreateStringValue(STRCONV(argv[15])));
				obj->SetNamedValue("ototalfinal", JsonValue::CreateNumberValue(std::strtod(argv[16], nullptr)));
				obj->SetNamedValue("onotes", JsonValue::CreateStringValue(STRCONV(argv[17])));
				obj->SetNamedValue("customerexpecteddate", JsonValue::CreateNumberValue(strtol(argv[18], nullptr, 10)));
				obj->SetNamedValue("owner", JsonValue::CreateStringValue(STRCONV(argv[19])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(SALESORDERS)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		osql.str("");
		//                0    1     2      3     4  5        6           7       8         9        10          11           12                 13
		osql << "select _id, _rev, state, soid, pid, pqty, unitprice, grossprice, pdisc, pdisctype, netprice, isexpedited, customerexpecteddate, owner "
			<< " from " << SALESORDERDETAILS << " where state in(1,2,3);";

		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("soid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("pid", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("pqty", JsonValue::CreateNumberValue(std::strtol(argv[5], nullptr, 10)));
				obj->SetNamedValue("unitprice", JsonValue::CreateNumberValue(std::strtod(argv[6], nullptr)));
				obj->SetNamedValue("grossprice", JsonValue::CreateNumberValue(std::strtod(argv[7], nullptr)));
				obj->SetNamedValue("pdisc", JsonValue::CreateNumberValue(std::strtod(argv[8], nullptr)));
				obj->SetNamedValue("pdisctype", JsonValue::CreateStringValue(STRCONV(argv[9])));
				obj->SetNamedValue("netprice", JsonValue::CreateNumberValue(std::strtod(argv[10], nullptr)));
				obj->SetNamedValue("isexpected", JsonValue::CreateStringValue(STRCONV(argv[11])));
				obj->SetNamedValue("customerexpecteddate", JsonValue::CreateNumberValue(strtol(argv[12], nullptr, 10)));
				obj->SetNamedValue("owner", JsonValue::CreateStringValue(STRCONV(argv[13])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(SALESORDERDETAILS)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		//               0      1    2      3     4        5           6       7
		osql << "select _id, _rev, state, soid, ostatus, ostatusdate, notes, createdby from " << SALESORDERSTATUSHISTORY << " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("soid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("ostatus", JsonValue::CreateNumberValue(std::strtol(argv[4], nullptr, 10)));
				obj->SetNamedValue("ostatusdate", JsonValue::CreateNumberValue(strtol(argv[5], nullptr, 10)));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("createdby", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(SALESORDERSTATUSHISTORY)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");

		//               0     1     2       3       4      5           6            7        8
		osql << "select _id, _rev, state, ositemid, oslid, osstatus, paymentstatus, notes, createdby from " << SALESORDERITEMSTATUSHISTORY << " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("ositemid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("oslid", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("osstatus", JsonValue::CreateNumberValue(std::strtol(argv[5], nullptr, 10)));
				obj->SetNamedValue("paymentstatus", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("createdby", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(SALESORDERITEMSTATUSHISTORY)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");

		//                0    1     2     3            4           5                  6            7        8            9
		osql << "select _id, _rev, state, soid, paymentstatus, paymentinstrument, paymentdate, currency, paidamount, paydetails from " << SALESORDERPAYMENTHISTORY << " where state in (1,2,3);";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("soid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("paymentstatus", JsonValue::CreateNumberValue(std::strtol(argv[4], nullptr, 10)));
				obj->SetNamedValue("paymentinstrument", JsonValue::CreateNumberValue(std::strtol(argv[5], nullptr, 10)));
				obj->SetNamedValue("paymentdate", JsonValue::CreateNumberValue(strtol(argv[6], nullptr, 10)));
				obj->SetNamedValue("currency", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("paidamount", JsonValue::CreateNumberValue(std::strtod(argv[8], nullptr)));
				obj->SetNamedValue("paydetails", JsonValue::CreateStringValue(STRCONV(argv[9])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(SALESORDERPAYMENTHISTORY)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");

		return;

	ONERROR:
		ThrowException(MessageIds::DBQueryFatalError);

	}

	void SqliteDataManager::GetCustomerLocalChanges(JsonArray ^ fill)
	{
		ostringstream osql;
		//                0     1   2      3       4    5     6         7        8       9       10
		osql << "select _id, _rev, state, ccode, name, url, facebook, linkedin, notes, isactive, logo from " << CUSTOMERORGS << " where state in (1,2,3)";
		int ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("ccode", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("url", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("facebook", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("linkedin", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("isactive", JsonValue::CreateBooleanValue(std::strtol(argv[9], nullptr, 2) ? true : false));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(CUSTOMERORGS)));
				//TODO Logo will be send as an attachment.
				//TODO How to handle LOGO attachments when deleted.

				// Putting an attachment (logo)
				//http://docs.couchdb.org/en/2.1.1/api/document/attachments.html?highlight=attachment
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}
		osql.str("");
		//                0    1    2       3      4      5      6                   7              8
		osql << "select _id, _rev, state, corgid, name, code, paymentmethodcode, paymenttermcode, invoicedesccode, ";
		//            9        10              11             12    13     14      15       16        17
		osql << "creditterm, creditlimit, shippingagentcode, phone, email, fax, mailaddr, billaddr, shipaddr, ";
		//           18        19      20          21
		osql << "searchname, notes, isactive, isdefault from " << CORGCOSTCENTER << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("corgid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("name", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("code", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("paymentmethodcode", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("paymenttermcode", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("invoicedesccode", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("creditterm", JsonValue::CreateNumberValue(strtol(argv[9], nullptr, 10)));
				obj->SetNamedValue("creditlimit", JsonValue::CreateNumberValue(strtod(argv[10], nullptr)));
				obj->SetNamedValue("shippingagentcode", JsonValue::CreateStringValue(STRCONV(argv[11])));
				obj->SetNamedValue("phone", JsonValue::CreateStringValue(STRCONV(argv[12])));
				obj->SetNamedValue("email", JsonValue::CreateStringValue(STRCONV(argv[13])));
				obj->SetNamedValue("fax", JsonValue::CreateStringValue(STRCONV(argv[14])));
				obj->SetNamedValue("mailaddr", JsonValue::CreateStringValue(STRCONV(argv[15])));
				obj->SetNamedValue("billaddr", JsonValue::CreateStringValue(STRCONV(argv[16])));
				obj->SetNamedValue("shipaddr", JsonValue::CreateStringValue(STRCONV(argv[17])));
				obj->SetNamedValue("searchname", JsonValue::CreateStringValue(STRCONV(argv[18])));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[19])));
				obj->SetNamedValue("isactive", JsonValue::CreateBooleanValue(STRCONV(argv[20])=="1" ? true : false));
				obj->SetNamedValue("isdefault", JsonValue::CreateBooleanValue(STRCONV(argv[21])=="1" ? true : false));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(CORGCOSTCENTER)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, corgccid, firstname, lastname, ";
		osql << "phone1, phone2, cellphone, email, facebook, linkedin from " << CUSTOMERORGCONTACTS << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("corgccid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("firstname", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("lastname", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("phone1", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("phone2", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("cellphone", JsonValue::CreateStringValue(STRCONV(argv[8])));
				obj->SetNamedValue("email", JsonValue::CreateStringValue(STRCONV(argv[9])));
				obj->SetNamedValue("facebook", JsonValue::CreateStringValue(STRCONV(argv[10])));
				obj->SetNamedValue("linkedin", JsonValue::CreateStringValue(STRCONV(argv[11])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(CUSTOMERORGCONTACTS)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		osql.str("");
		osql << "select _id, _rev, state, uid, corgccid, scode, srole, notes from " << FIELDREP << " where state in (1,2,3)";
		ret = ExecSqlNative(osql.str(), [fill](void* p, int argc, char** argv, char** azColName)
		{
			auto state = (DataState)std::stoi(argv[2]);
			JsonObject ^ obj = ref new JsonObject();
			obj->SetNamedValue("_id", JsonValue::CreateStringValue(STRCONV(argv[0])));
			if ((DataState::Edited == state || DataState::Deleted == state) && argv[1])
				obj->SetNamedValue("_rev", JsonValue::CreateStringValue(STRCONV(argv[1])));
			if (state == DataState::Deleted)
				obj->SetNamedValue("_deleted", JsonValue::CreateBooleanValue(true));
			else
			{
				obj->SetNamedValue("uid", JsonValue::CreateStringValue(STRCONV(argv[3])));
				obj->SetNamedValue("corgccid", JsonValue::CreateStringValue(STRCONV(argv[4])));
				obj->SetNamedValue("scode", JsonValue::CreateStringValue(STRCONV(argv[5])));
				obj->SetNamedValue("srole", JsonValue::CreateStringValue(STRCONV(argv[6])));
				obj->SetNamedValue("notes", JsonValue::CreateStringValue(STRCONV(argv[7])));
				obj->SetNamedValue("type", JsonValue::CreateStringValue(StringHelpers::ConvertToString(FIELDREP)));
			}
			fill->Append(obj);
			return SQLITE_OK;
		}, nullptr);
		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		//ret = DeleteLocalItems({ CUSTOMERORGS, CORGCOSTCENTER, CUSTOMERORGCONTACTS, FIELDREP });

		if (!IsSqlOk(ret))
		{
			goto ONERROR;
		}

		return;

	ONERROR:
		ThrowException(MessageIds::DBQueryFatalError);

	}

	task<JsonArray^> SqliteDataManager::FetchLocalChangesAsync()
	{
		return create_task([this]()
		{
			return FetchLocalChanges();
		});
	}

	JsonArray^ SqliteDataManager::FetchLocalChanges()
	{
		JsonArray^ changes = ref new JsonArray();
		BeginTrans();
		this->GetCommonDataLocalChanges(changes);
		this->GetProdCategoryLocalChanges(changes);
		this->GetProductLocalChanges(changes);
		this->GetInventoryLocalChanges(changes);
		this->GetCustomerLocalChanges(changes);
		this->GetOrdersLocalChanges(changes);
		CommitTrans();
		return changes;
	}

	DataState SqliteDataManager::UpdateAppSettings(JsonObject ^ obj)
	{
		DataState state = DataState::Unmodified;
		if (obj != nullptr)
		{
			ostringstream osql;
			String^ rev = obj->GetNamedString("_rev", nullptr);
			if (rev != nullptr)
			{
				osql << "insert or replace into appsettings (propname, propvalue, isroaming, state) "
					<< " values "
					<< "('_rev', '" << STRCONV(rev) << "', 1, 0);";
			}
			/* Sample
			{
				"_id":"charan@empire.com",
				"type":"appsettings",
				"props":[{"usercrccode":"54"},{"customersorg_CodeField":"0,0,0,0,2"}]
			}
			*/
			auto arr = obj->GetNamedArray("props", nullptr);
			if (arr != nullptr)
			{
				for (IJsonValue^ mem : arr)
				{
					auto jobj = (JsonObject^)mem->GetObject();
					String^ propName = jobj->GetNamedString("propname", nullptr);
					String^ propVal = jobj->GetNamedString("propval", "");
					if (propName != nullptr)
					{
						osql << "insert or replace into appsettings (propname, propvalue, isroaming, state) "
							<< " values "
							<< "('" << STRCONV(propName) << "', '" << STRCONV(propVal) << "', 1, 0);";
					}
				}
			}
			if (!IsSqlOk(ExecSql(osql.str())))
			{
				state = DataState::Error;
			}
		}
		return state;
	}

	DataState SqliteDataManager::UpdateEnumTable(String ^ tableName, JsonObject ^ obj)
	{
		DataState state = DataState::Unmodified;
		if (ISEMPTYSTR(tableName)) return state;
		if (nullptr == obj) return state;
		auto items = obj->GetNamedArray("items", nullptr);
		if (items != nullptr)
		{
			LPSTR pkey = nullptr;
			if (tableName == ORDERSTATUS)
				pkey = "osid";
			else if (tableName == PAYMENTSTATUS)
				pkey = "psid";
			else if (tableName == PAYMENTINSTRUMENT)
				pkey = "pmid";
			else if (tableName == INVENTORYMOVEMENTTYPE)
				pkey = "invmvid";
			if (pkey)
			{
				ostringstream osql;
				for (auto item : items)
				{
					JsonObject^ jitem = item->GetObject();
					auto id = jitem->GetNamedNumber(StringHelpers::ConvertToString(pkey), 0);
					auto priv = STRSAFECONV(jitem->GetNamedString("privatename", ""));
					auto publ = STRSAFECONV(jitem->GetNamedString("publicname", ""));
					auto desc = STRSAFECONV(jitem->GetNamedString("desc", ""));
					osql << "insert or replace into " << STRCONV(tableName)
						<< "(" << pkey << ", state, privatename, publicname, desc) "
						<< "values "
						<< "( " << id << ", 0, '" << priv << "', '" << publ << "', '" << desc << "' );";
				}
				if (!osql.str().empty())
				{
					if (!IsSqlOk(ExecSql(osql.str())))
					{
						state = DataState::Error;
					}
				}
			}
		}
		return state;
	}

	DataState SqliteDataManager::UpdateAddress(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << ADDRESSES << " WHERE _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{

			// make sure essential properties are available.
			String^ id = obj->GetNamedString("_id");
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string line1 = STRSAFECONV(obj->GetNamedString("line1"));
			const string line2 = STRSAFECONV(obj->GetNamedString("line2", ""));
			const string city = STRSAFECONV(obj->GetNamedString("city", ""));
			const string province = STRSAFECONV(obj->GetNamedString("province", ""));
			const string postalcode = STRSAFECONV(obj->GetNamedString("postalcode", ""));
			const string country = STRSAFECONV(obj->GetNamedString("country", ""));

			std::string  crit = "_Id='" + STRCONV(id->Data()) + "'";
			osql << "INSERT OR REPLACE INTO " << ADDRESSES << " ";
			osql << "( _id, _rev, state, line1, line2, city, province, postalcode, country )";
			osql << " VALUES (";
			osql << "'" << STRCONV(id) << "', '" << _rev << "', 0, '" << line1 << "', '" << line2 << "',";
			osql << "'" << city << "', '" << province << "', '" << postalcode << "', '" << country << "' );";
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateCategory(JsonObject^ obj, bool deleteit)
	{
		wstringstream osql;
		DataState state = DataState::Unmodified;
		if (!deleteit)
		{
			wstring id, rev, name, desc;
			id = obj->GetNamedString(L"_id")->Data();
			rev = obj->GetNamedString(L"_rev")->Data();
			name = obj->GetNamedString(L"name", L"")->Data();
			desc = obj->GetNamedString(L"desc", L"")->Data();
			osql << "UPDATE " << PRODUCTCATEGORIES << " SET _rev = '" << rev << "', ";
			osql << "name = '" << name << "', desc = '" << desc << "', state = 0 ";
			osql << "WHERE _id = '" << id << "';";
			osql << "INSERT INTO " << PRODUCTCATEGORIES;
			osql << " ( _id, _rev, name, desc ) ";
			osql << " SELECT '" << id << "', '" << rev << "', '";
			osql << name << "', '" << desc << "' ";
			osql << " WHERE (Select Changes() = 0);";
			state = DataState::Edited; // TODO. return Added or Edited
		}
		else
		{
			osql << "DELETE " << PRODUCTCATEGORIES << " WHERE _id = '" << obj->GetNamedString(L"_id")->Data() << "';";
			state = DataState::Deleted; 
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateCategoryToCategoryRelations(JsonObject ^ dataobj, bool deleteit)
	{
		DataState state = DataState::Unmodified;
		wstringstream osql;
		if (deleteit)	// this approach doesn't notify the gui api about the object removed. some kind of subscription is needed.
		{
			osql << "DELETE " << PRODUCTCATEGORYCATEGORYREL << " WHERE _id = '" << dataobj->GetNamedString(L"_id")->Data() << "';";
			state = DataState::Deleted;
		}
		else
		{
			osql << "UPDATE " << PRODUCTCATEGORYCATEGORYREL << " SET _rev = '" << dataobj->GetNamedString(L"_rev")->Data() << "', ";
			osql << "cid = '" << dataobj->GetNamedString(L"cid")->Data() << "', ";
			osql << "pcid = '" << dataobj->GetNamedString(L"pcid")->Data() << "', state = 0 ";
			osql << "WHERE _id ='" << dataobj->GetNamedString(L"_id")->Data() << "';";
			osql << "INSERT INTO " << PRODUCTCATEGORYCATEGORYREL << " (_id, _rev, cid, pcid) ";
			osql << "SELECT '" << dataobj->GetNamedString(L"_id")->Data() << "', ";
			osql << "'" << dataobj->GetNamedString(L"_rev")->Data() << "', ";
			osql << "'" << dataobj->GetNamedString(L"cid")->Data() << "', ";
			osql << "'" << dataobj->GetNamedString(L"pcid")->Data() << "' WHERE (SELECT Changes() = 0);";
			state = DataState::Edited; // TODO. return Added or Edited
		}
		// https://stackoverflow.com/questions/5285456/android-sqlite-error-code-21

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateProductFamily(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << PRODUCTFAMILIES << " WHERE _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{

			// make sure essential properties are available.
			String^ id = obj->GetNamedString("_id");
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string name = STRSAFECONV(obj->GetNamedString("name"));
			const string desc = STRSAFECONV(obj->GetNamedString("desc", ""));
			const string shortdesc = STRSAFECONV(obj->GetNamedString("shortdesc", ""));
			const string notes = STRSAFECONV(obj->GetNamedString("notes", ""));
			const string pdefattrid = STRCONV(obj->GetNamedString("pdefattrid", ""));

			osql << "INSERT OR REPLACE INTO " << PRODUCTFAMILIES << " ";
			osql << "( _id, _rev, state, name, desc, shortdesc, notes, pdefattrid )";
			osql << " VALUES (";
			osql << "'" << STRCONV(id) << "', '" << _rev << "', 0, '" << name << "', '" << desc << "',";
			osql << "'" << shortdesc << "', '" << notes << "', '" << pdefattrid << "' )";
		}

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateProductAttribute(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << PRODUCTATTRIBUTE << " WHERE _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string paid = STRCONV(obj->GetNamedString("paid"));
			const string name = STRSAFECONV(obj->GetNamedString("name"));
			const string desc = STRSAFECONV(obj->GetNamedString("desc", ""));
			const string attrtype = STRCONV(obj->GetNamedString("attrtype"));
			osql << "insert or replace into " << PRODUCTATTRIBUTE << " ( "
				<< "_id, _rev, state, paid, name, desc, attrtype ) "
				<< " VALUES ( "
				<< "'" << id << "','" << _rev << "', 0 , '" << paid << "', '" << name << "', '" << desc << "', '" << attrtype << "' "
				<< ");";
		}

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateProductAttributeValue(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << PRODUCTATTRIBUTEVALUE << " WHERE _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string pid = STRCONV(obj->GetNamedString("pid"));
			const string aid = STRCONV(obj->GetNamedString("aid"));
			const string avalue = STRSAFECONV(obj->GetNamedString("avalue"));
			const string avaluetype = STRCONV(obj->GetNamedString("avaluetype"));
			const string uom = STRSAFECONV(obj->GetNamedString("uom"));
			osql << "insert or replace into " << PRODUCTATTRIBUTEVALUE << " ( "
				<< "_id, _rev, state, pid, aid, avalue, avaluetype, uom ) "
				<< " VALUES ( "
				<< "'" << id << "','" << _rev << "', 0, '" << pid << "', '" << aid << "', '" << avalue << "', '" << avaluetype << "', '" << uom << "' "
				<< ");";
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateInventoryLocation(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string rev = STRCONV(obj->GetNamedString("_rev", ""));
		const string location = STRCONV(obj->GetNamedString("location"));
		const string code = STRCONV(obj->GetNamedString("code"));
		const string contactid = STRCONV(obj->GetNamedString("contactid"));
		const string addressid = STRCONV(obj->GetNamedString("addressid"));
		osql << "insert or replace into " << INVENTORYLOCATION
			<< "(_id, _rev, state, location, code, contactid, addressid) "
			<< " values ( "
			<< "'" << id << "', '" << rev << "', 0, '" << location << "', '" << code << "', '" << contactid << "', '" << addressid << "' "
			<< "); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateActiveInventory(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;

		const string id = STRCONV(obj->GetNamedString("_id"));
		const string rev = STRCONV(obj->GetNamedString("_rev", ""));
		const string pid = STRCONV(obj->GetNamedString("pid"));
		const long pqty = (long)obj->GetNamedNumber("pqty", 0L);
		const string loc = STRCONV(obj->GetNamedString("location", ""));

		osql << "insert or replace into " << ACTIVEINVENTORY
			<< "(_id, _rev, state, pid, pqty, location) "
			<< " values ( "
			<< "'" << id << "', '" << rev << "', 0, '" << pid << "', " << pqty << ", '" << loc << "' "
			<< "); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateInventoryProperty(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;

		const string id = STRCONV(obj->GetNamedString("_id"));
		const string rev = STRCONV(obj->GetNamedString("_rev", ""));
		const string pid = STRCONV(obj->GetNamedString("pid"));
		const long minqty = (long)obj->GetNamedNumber("minqty", 0L);
		const long maxqty = (long)obj->GetNamedNumber("maxqty", 0L);
		const long refilltype = (long)obj->GetNamedNumber("refilltype", 0L);

		osql << "insert or replace into " << INVENTORYPROPERTY
			<< "(_id, _rev, state, pid, minqty, maxqty, refilltype) "
			<< " values ( "
			<< "'" << id << "', '" << rev << "', 0, '" << pid << "', " << minqty << ", " << maxqty << ", " << refilltype 
			<< " ); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateInventoryTrx(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;

		const string id = STRCONV(obj->GetNamedString("_id"));
		const string rev = STRCONV(obj->GetNamedString("_rev", ""));
		const string pid = STRCONV(obj->GetNamedString("pid"));
		const long idate = obj->GetNamedNumber("idate", 0);
		const long qtymoved = (long)obj->GetNamedNumber("qtymoved", 0L);
		const long movetype= (long)obj->GetNamedNumber("movetype", 0L);
		const long movedir = (long)obj->GetNamedNumber("movedir", 0L);
		const string inouttag = STRCONV(obj->GetNamedString("inouttag", ""));

		osql << "insert or replace into " << INVENTORYTRX
			<< "(_id, _rev, state, pid, idate, qtymoved, movetype, movedir, inouttag ) "
			<< " values ( "
			<< "'" << id << "', '" << rev << "', 0, '" << pid << "', " << idate << ", " << qtymoved << ", "
			<< movetype << ", " << movedir << ", '" << inouttag << "' "
			<< "); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateCustomerOrg(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << CUSTOMERORGS << " WHERE state = " << (int)DataState::Deleted << " _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string ccode = STRSAFECONV(obj->GetNamedString("ccode", ""));
			const string name = STRSAFECONV(obj->GetNamedString("name"));
			const string url = STRSAFECONV(obj->GetNamedString("url", ""));
			const string facebook = STRSAFECONV(obj->GetNamedString("facebook", ""));
			const string linkedin = STRSAFECONV(obj->GetNamedString("linkedin", ""));
			const string notes = STRSAFECONV(obj->GetNamedString("notes", ""));
			const bool isactive = obj->GetNamedBoolean("isactive", false);
			//TODO How to obtain logo attachment. This could be a separate call.

			if (HasData(CUSTOMERORGS, "_id = '" + id + "'"))
			{
				osql << "update " << CUSTOMERORGS << " SET "
					<< "_rev = '" << _rev << "', "
					<< "state = " << 0 << ", "
					<< "ccode = '" << ccode << "', "
					<< "name = '" << name << "', "
					<< "url = '" << url << "', "
					<< "facebook = '" << facebook << "', "
					<< "linkedin = '" << linkedin << "', "
					<< "notes = '" << notes << "', "
					<< "isactive = " << (isactive ? 1 : 0) << " "
					<< " where _id = '" << id << "';";
				//TODO How to handle LOGO attachments .
			}
			else
			{
				osql << "insert into " << CUSTOMERORGS << " ( "
					<< "_id, _rev, state, ccode, name, url, facebook, linkedin, notes, isactive ) "
					<< " VALUES ( "
					<< "'" << id << "','" << _rev << "', 0, '" << ccode << "', '" << name << "', '" << url << "', '" << facebook << "', '" << linkedin << "', '" << notes << "', " << (isactive ? 1 : 0)
					<< ");";
				//TODO How to handle LOGO attachments .
			}
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateCOrgCostCenter(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << CORGCOSTCENTER << " WHERE state = " << (int)DataState::Deleted << " _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string corgid = STRCONV(obj->GetNamedString("corgid"));
			const string name = STRSAFECONV(obj->GetNamedString("name"));
			const string code = STRSAFECONV(obj->GetNamedString("code", ""));
			const string paymentmethodcode = STRSAFECONV(obj->GetNamedString("paymentmethodcode", ""));
			const string paymenttermcode = STRCONV(obj->GetNamedString("paymetntermcode", ""));
			const string invoicedesccode = STRSAFECONV(obj->GetNamedString("invoicedesccode", ""));
			const int creditterm = (int)obj->GetNamedNumber("creditterm", 0);
			const auto creditlimit = obj->GetNamedNumber("creditlimit", 0);
			const string shippingagentcode = STRSAFECONV(obj->GetNamedString("shippingagentcode", ""));
			const string phone = STRSAFECONV(obj->GetNamedString("phone", ""));
			const string email = STRSAFECONV(obj->GetNamedString("email", ""));
			const string fax = STRSAFECONV(obj->GetNamedString("fax", ""));
			const string mailaddr = STRCONV(obj->GetNamedString("mailaddr", ""));
			const string billaddr = STRCONV(obj->GetNamedString("billaddr", ""));
			const string shipaddr = STRCONV(obj->GetNamedString("shipaddr", ""));
			const string searchname = STRCONV(obj->GetNamedString("searchname", ""));
			const string notes = STRCONV(obj->GetNamedString("notes", ""));
			const bool isactive = obj->GetNamedBoolean("isactive", false);
			const bool isdefault = obj->GetNamedBoolean("isdefault", false);

			if (HasData(CORGCOSTCENTER, "_id = '" + id + "'"))
			{
				osql << "update " << CORGCOSTCENTER << " SET "
					<< "_rev = '" << _rev << "', "
					<< "state = " << 0 << ", "
					<< "corgid = '" << corgid << "', "
					<< "name = '" << name << "', "
					<< "code = '" << code << "', "
					<< "paymentmethodcode = '" << paymentmethodcode << "', "
					<< "paymenttermcode = '" << paymenttermcode << "', "
					<< "invoicedesccode = '" << invoicedesccode << "', "
					<< "creditterm = " << creditterm << ", "
					<< "creditlimit = " << creditlimit << ", "
					<< "shippingagentcode = '" << shippingagentcode << "', "
					<< "phone = '" << phone << "', "
					<< "email = '" << email << "', "
					<< "fax = '" << fax << "', "
					<< "mailaddr = '" << mailaddr << "', "
					<< "billaddr = '" << billaddr << "', "
					<< "shipaddr = '" << shipaddr << "', "
					<< "searchname = '" << searchname << "', "
					<< "notes = '" << notes << "', "
					<< "isactive = " << (isactive ? 1 : 0) << ", "
					<< "isdefault = " << (isdefault ? 1 : 0) 
					<< " where _id = '" << id << "';";
			}
			else
			{
				osql << "insert into " << CORGCOSTCENTER << " ( "
					<< "_id, _rev, state, corgid, name, code, paymentmethodcode, paymenttermcode, invoicedesccode, creditterm, "
					<< "creditlimit, shippingagentcode, phone, email, fax, "
					<< "mailaddr, billaddr, shipaddr, searchname, notes, isactive, isdefault ) "
					<< " VALUES ( "
					<< "'" << id << "','" << _rev << "', 0, '" << corgid << "', '" << name << "', '" << code << "', '" << paymentmethodcode
					<< "', '" << paymenttermcode << "', '" << invoicedesccode << "', " << creditterm << ", " << creditlimit
					<< ", '" << shippingagentcode << "', '" << phone << "', '" << email << "', '"
					<< fax << "', '" << mailaddr << "', '" << billaddr << "', '" << shipaddr << "', '"
					<< searchname << "', '" << notes << "', " << (isactive ? 1 : 0) << ", " << (isdefault ? 1 : 0)
					<< " );";
			}
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateCustomerOrgContact(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << CUSTOMERORGCONTACTS << " WHERE state = " << (int)DataState::Deleted << " _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string corgccid = STRCONV(obj->GetNamedString("corgccid"));
			const string firstname = STRSAFECONV(obj->GetNamedString("firstname"));
			const string lastname = STRSAFECONV(obj->GetNamedString("lastname"));
			const string phone1 = STRCONV(obj->GetNamedString("phone1"));
			const string phone2 = STRSAFECONV(obj->GetNamedString("phone2"));
			const string cellphone = STRSAFECONV(obj->GetNamedString("cellphone", ""));
			const string email = STRSAFECONV(obj->GetNamedString("email", ""));
			const string facebook = STRSAFECONV(obj->GetNamedString("facebook", ""));
			const string linkedin = STRSAFECONV(obj->GetNamedString("linkedin", ""));

			// based on couchdb documentation, if the doucment contains one or more attachments, 
			// the doc will have "_attachments" property as below url example shows.
			// http://docs.couchdb.org/en/2.1.1/replication/protocol.html?highlight=attachment


			if (HasData(CUSTOMERORGCONTACTS, "_id = '" + id + "'"))
			{
				osql << "update " << CUSTOMERORGCONTACTS << " SET "
					<< "_rev = '" << _rev << "', "
					<< "state = " << 0 << ", "
					<< "corgccid = '" << corgccid << "', "
					<< "firstname = '" << firstname << "', "
					<< "lastname = '" << lastname << "', "
					<< "phone1 = '" << phone1 << "', "
					<< "phone2 = '" << phone2 << "', "
					<< "cellphone = '" << cellphone << "', "
					<< "email = '" << email << "', "
					<< "facebook = '" << facebook << "', "
					<< "linkedin = '" << linkedin << "' "
					<< " where _id = '" << id << "';";
			}
			else
			{
				osql << "insert into " << CUSTOMERORGCONTACTS << " ( "
					<< "_id, _rev, state, corgccid, firstname, lastname, phone1, phone2, cellphone, email, facebook, linkedin ) "
					<< " VALUES ( "
					<< "'" << id << "', '" << _rev << "', 0, '" << corgccid << "', '" << firstname << "', '" << lastname
					<< "', '" << phone1 << "', '" << phone2 << "', '" << cellphone << "', '" << email << "', '"
					<< facebook << "', '" << linkedin << "' "
					<< ");";
			}
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateFieldReps(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << FIELDREP << " WHERE state = " << (int)DataState::Deleted << " _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string _rev = STRCONV(obj->GetNamedString("_rev"));
			const string uid = STRCONV(obj->GetNamedString("sid"));
			const string corgccid = STRSAFECONV(obj->GetNamedString("corgccid"));
			const string scode = STRSAFECONV(obj->GetNamedString("scode"));
			const string srole = STRCONV(obj->GetNamedString("srole"));
			const string notes= STRSAFECONV(obj->GetNamedString("notes"));

			if (HasData(FIELDREP, "_id = '" + id + "'"))
			{
				osql << "update " << FIELDREP << " SET "
					<< "_rev = '" << _rev << "', "
					<< "state = " << 0 << ", "
					<< "uid = '" << uid << "', "
					<< "corgccid = '" << corgccid << "', "
					<< "scode = '" << scode << "', "
					<< "srole = '" << srole << "', "
					<< "notes = '" << notes << ", "
					<< " where _id = '" << id << "';";
			}
			else
			{
				osql << "insert into " << FIELDREP << " ( "
					<< "_id, _rev, state, uid, corgccid, scode, srole, notes ) "
					<< " VALUES ( "
					<< "'" << id << "','" << _rev << "', 0, '" << uid << "', '" << corgccid << "', '" << scode
					<< "', '" << srole << "', '" << notes << "' "
					<< ");";
			}
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateSalesOrder(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string _rev = STRCONV(obj->GetNamedString("_rev"));
		const string onum = STRSAFECONV(obj->GetNamedString("onumber"));
		const string corgccid = STRCONV(obj->GetNamedString("corgccid", ""));
		const string batchno = STRSAFECONV(obj->GetNamedString("batchno", ""));
		const long odate = obj->GetNamedNumber("odate");
		//const string otime = STRSAFECONV(obj->GetNamedString("otime"));
		const string cccontactid = STRCONV(obj->GetNamedString("cccontactid", ""));
		const string repid = STRCONV(obj->GetNamedString("repid", ""));
		const double ototalgross = obj->GetNamedNumber("ototalgross", 0.0);
		const string disccode = STRSAFECONV(obj->GetNamedString("disccode", ""));
		const double discnum = obj->GetNamedNumber("discnum", 0.0);
		const string disctype = STRSAFECONV(obj->GetNamedString("disctype", "")); // "%" or "#"
		const double taxpct = obj->GetNamedNumber("discnum", 0.0);
		const double taxvalue = obj->GetNamedNumber("discnum", 0.0);
		const string currencyid = STRCONV(obj->GetNamedString("currencyid", ""));
		const double ototalfinal = obj->GetNamedNumber("ototalfinal", 0.0);
		const string onotes = STRSAFECONV(obj->GetNamedString("onotes", ""));
		const long customerexpecteddate = obj->GetNamedNumber("customerexpecteddate", 0);
		const string owner = STRSAFECONV(obj->GetNamedString("owner", "")); // email address of user.

		osql << "insert or replace into " << SALESORDERS << " ( "
			<< "_id, _rev, state, onumber, corgccid, batchno, odate, cccontactid, repid, ototalgross, "
			<< "disccode, discnum, disctype, taxpct, taxvalue, currencyid, ototalfinal, onotes, customerexpecteddate, owner )"
			<< " VALUES ( "
			<< "'" << id << "','" << _rev << "', 0, '" << onum << "', '" << corgccid << "', '" << batchno << "', "
			<< odate << ", '" << cccontactid << "', '" << repid << "', " << ototalgross << ", '"
			<< disccode << "', " << discnum << ", '" << disctype << "', " << taxpct << ", " << taxvalue << ", '" << currencyid << "', " << ototalfinal << ", '"
			<< onotes << "', " << customerexpecteddate << ", '" << owner << "' "
			<< ");";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateSalesOrderDetails(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string _rev = STRCONV(obj->GetNamedString("_rev"));
		const string soid = STRCONV(obj->GetNamedString("soid", ""));
		if (soid.empty())
			return DataState::Error;
		const string pid = STRCONV(obj->GetNamedString("pid"));
		const long pqty = obj->GetNamedNumber("pqty");
		const double unitprice = obj->GetNamedNumber("unitprice");
		const double grossprice = obj->GetNamedNumber("grossprice");
		const double pdisc = obj->GetNamedNumber("pdisc", 0.0);
		const string pdisctype = STRSAFECONV(obj->GetNamedString("pdisctype", ""));
		const double netprice = obj->GetNamedNumber("netprice", 0.0);
		int isexpedited = 0;
		auto objexpedited = obj->GetNamedValue("isexpedited", JsonValue::CreateBooleanValue(false));
		if (objexpedited->ValueType == JsonValueType::Boolean)
			isexpedited = objexpedited->GetBoolean() ? 1 : 0;
		else if (objexpedited->ValueType == JsonValueType::Number)
			isexpedited = objexpedited->GetNumber() == 0 ? 0 : 1;
		const long customerexpecteddate = obj->GetNamedNumber("customerexpecteddate", 0);
		const string owner = STRSAFECONV(obj->GetNamedString("owner", "")); // email address of user.

		osql << "insert or replace into " << SALESORDERDETAILS
			<< "( _id, _rev, state, soid, pid, pqty, unitprice, grossprice, pdisc, pdisctype, netprice, isexpedited, customerexpecteddate, owner )"
			<< " VALUES "
			<< "('" << id << "', '" << _rev << "', 0, '" << soid << "', '" << pid << "', " << pqty << ", " << unitprice << ", " << grossprice << ", " << pdisc << ", '" << pdisctype << "', " << netprice << ", " << isexpedited << ", " << customerexpecteddate << ", '" << owner << "' ); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateSalesOrderStatusHistory(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string _rev = STRCONV(obj->GetNamedString("_rev"));
		const string soid = STRCONV(obj->GetNamedString("soid"));
		auto statusobj = obj->GetNamedValue("ostatus", nullptr);
		int ostatus;
		if (statusobj != nullptr)
		{
			if (statusobj->ValueType == JsonValueType::String)
			{
				ostatus = strtol(STRCONV(obj->GetNamedString("ostatus", "0")->Data()).c_str(), nullptr, 10);
			}
			else if (statusobj->ValueType == JsonValueType::Number)
			{
				ostatus = obj->GetNamedNumber("ostatus", 0);
			}
		}
		const long ostatusdate = obj->GetNamedNumber("ostatusdate");
		const string notes = STRSAFECONV(obj->GetNamedString("notes", ""));
		const string createdby = STRCONV(obj->GetNamedString("createdby", ""));

		osql << "insert or replace into " << SALESORDERSTATUSHISTORY
			<< "( _id, _rev, state, soid, ostatus, ostatusdate, notes, createdby )"
			<< " VALUES "
			<< "('" << id << "', '" << _rev << "', 0, '" << soid << "', " << ostatus << ", " << ostatusdate << ", '" << notes << "', '" << createdby << "'); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateSalesOrderStatusItemHistory(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string _rev = STRCONV(obj->GetNamedString("_rev"));
		const string ositemid = STRCONV(obj->GetNamedString("ositemid"));
		const string oslid= STRCONV(obj->GetNamedString("oslid"));
		const int osstatus = (int)obj->GetNamedNumber("osstatus", 0);
		const int paymentstatus = (int)obj->GetNamedNumber("paymentstatus", 0);
		const string notes = STRSAFECONV(obj->GetNamedString("notes", ""));
		const string createdby = STRCONV(obj->GetNamedString("createdby"));

		osql << "insert or replace into " << SALESORDERITEMSTATUSHISTORY
			<< "( _id, _rev, state, ositemid, oslid, osstatus, paymentstatus, notes, createdby )"
			<< " VALUES "
			<< "('" << id << "', '" << _rev << "', 0, '" << ositemid << "', '" << oslid << "', " << osstatus << ", " << paymentstatus << ", '" << notes << "', '" << createdby << "'); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateSalesOrderPaymentHistory(JsonObject ^ obj)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string _rev = STRCONV(obj->GetNamedString("_rev"));
		const string soid = STRCONV(obj->GetNamedString("soid"));
		const int paymentstatus = (int)obj->GetNamedNumber("paymentstatus", 0);
		const int paymentinstrument = (int)obj->GetNamedNumber("paymentinstrument", 0);
		const long paymentdate = obj->GetNamedNumber("paymentdate");
		const string currency = STRSAFECONV(obj->GetNamedString("currency", ""));
		const double paidamount = obj->GetNamedNumber("paidamount");
		const string paydetails = STRSAFECONV(obj->GetNamedString("currency", ""));

		osql << "insert or replace into " << SALESORDERPAYMENTHISTORY
			<< "( _id, _rev, state, soid, paymentstatus, paymentinstrument, paymentdate, currency, paidamount, paydetails )"
			<< " VALUES "
			<< "('" << id << "', '" << _rev << "', 0, '" << soid << "', " << paymentstatus << ", " << paymentinstrument << ", " << paymentdate << ", '" << currency << "', " << paidamount << ", '" << paydetails << "'); ";

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateAppBusinessOrg(JsonObject ^ obj)
	{
		DataState state = DataState::Unmodified;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string _rev = STRCONV(obj->GetNamedString("_rev"));
		obj->Remove("_id");
		obj->Remove("_rev");
		const string objdata = STRCONV(obj->ToString());
		ostringstream osql;
		osql << "insert or replace into " << APPBUSINESSORG
			<< " (_id, _rev, state, objdata ) "
			<< " VALUES "
			<< "('" << id << "', '" << _rev << "', 0, '" << objdata << "' );";
		if (!IsSqlOk(ExecSql(osql.str())))
			state = DataState::Error;
		return state;
	}

	const char * SqliteDataManager::GetSqliteError(int result)
	{
		auto itr = sqliteErrors.find(result);
		if (itr != sqliteErrors.end())
			return itr->second;
		return "Sqlite Unexected error";
	}

	DataState SqliteDataManager::UpdateProduct(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << PRODUCTTABLE << " WHERE _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			// make sure essential properties are available.
			if (obj->HasKey("skun"))
			{
				String^ id = obj->GetNamedString("_id");
				const string _rev = STRCONV(obj->GetNamedString("_rev"));
				const string skun = STRCONV(obj->GetNamedString("skun"));
				const string name = STRCONV(obj->GetNamedString("name"));
				const string subname = STRCONV(obj->GetNamedString("subname"));
				const string desc = STRCONV(obj->GetNamedString("desc"));
				const string shortdesc = STRCONV(obj->GetNamedString("shortdesc"));
				const string notes = STRCONV(obj->GetNamedString("notes"));
				const string familyid = STRCONV(obj->GetNamedString("pfamilyid"));
				const string defattrid = STRCONV(obj->GetNamedString("pdefattrid", ""));
				const string priceplanid = STRCONV(obj->GetNamedString("priceplanid"));
				const string barcode = STRCONV(obj->GetNamedString("barcode"));
				bool isstockless = obj->GetNamedBoolean("isstockless");
				const string uom = STRCONV(obj->GetNamedString("uom"));

				std::string  crit = "_Id='" + STRCONV(id->Data()) + "'";
				if (HasData(PRODUCTTABLE, crit))
				{
					osql << "UPDATE " << PRODUCTTABLE << " SET ";
					osql << "_rev = '" << _rev << "', ";
					osql << "state = " << 0 << ", ";
					osql << "skun = '" << skun << "', ";
					osql << "name = '" << name << "', ";
					osql << "subname = '" << subname << "', ";
					osql << "desc = '" << desc << "', ";
					osql << "shortdesc = '" << shortdesc << "', ";
					osql << "pfamilyid = '" << familyid << "', ";
					osql << "pdefattrid = '" << defattrid << "', ";
					osql << "priceplanid = '" << priceplanid << "', ";
					osql << "barcode = '" << barcode << "', ";
					osql << "isstockless = " << (isstockless ? 1 : 0) << ", ";
					osql << "uom = '" << uom << "' ";
					osql << "where _id = '" << STRCONV(id) << "';";
					state = DataState::Edited;
				}
				else
				{
					osql << "INSERT INTO " << PRODUCTTABLE << " ";
					osql << "( _id, _rev, state, skun, name, subname, desc, shortdesc, notes, pfamilyid, pdefattrid, priceplanid, barcode, isstockless, uom )";
					osql << " VALUES (";
					osql << "'" << STRCONV(id) << "', '" << _rev << "', " << 0 << ", '" << skun << "', '" << name << "', '" << subname << "', '" << desc << "',";
					osql << "'" << shortdesc << "', '" << notes << "', '" << familyid << "','" << defattrid << "' , '" << priceplanid << "', '" << barcode << "', ";
					osql << (isstockless ? 1 : 0) << ", '" << uom << "'";
					osql << ")";
				}
			}
			else
			{
				OutputDebugString(L"Invalid product information.");
				return state;
			}
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateCategoryToProduct(JsonObject ^ dataobj, bool deleteit)
	{
		DataState state = DataState::Unmodified;
		wstringstream osql;
		if (deleteit)	// this approach doesn't notify the gui api about the object removed. some kind of subscription is needed.
		{
			osql << "DELETE " << CATEGORYTOPRODUCT << " WHERE _id = '" << dataobj->GetNamedString(L"_id")->Data() << "';";
			state = DataState::Deleted;
		}
		else
		{
			wstring id = dataobj->GetNamedString(L"_id")->Data();
			wstring pid = dataobj->GetNamedString(L"pid")->Data();
			wstring cid = dataobj->GetNamedString(L"cid")->Data();
			osql.str(L"");
			// delete any product relation when product doesn't exist
			osql << L"pid ='" << pid << "' AND ";
			osql << L"cid = '" << cid << "'";
			string rowId = GetRowKey(CATEGORYTOPRODUCT, STRCONV(osql.str()));
			if (!rowId.empty())
			{
				osql.str(L"");
				if (rowId != STRCONV(id))
				{
					osql << "DELETE FROM " << CATEGORYTOPRODUCT << " WHERE _id = '" << id << "';";
				}
				osql << "UPDATE " << CATEGORYTOPRODUCT;
				osql << " SET _rev = '" << dataobj->GetNamedString(L"_rev")->Data() << "', ";
				osql << "cid = '" << dataobj->GetNamedString(L"cid")->Data() << "', ";
				osql << "pid = '" << dataobj->GetNamedString(L"pid")->Data() << "', state = 0 ";
				osql << "WHERE _id ='" << STRCONV(rowId) << "';";
			}
			else
			{
				osql.str(L"");
				osql << "INSERT INTO " << CATEGORYTOPRODUCT << " (_id, _rev, cid, pid) ";
				osql << "SELECT '" << id << "', ";
				osql << "'" << dataobj->GetNamedString(L"_rev")->Data() << "', ";
				osql << "'" << dataobj->GetNamedString(L"cid")->Data() << "', ";
				osql << "'" << dataobj->GetNamedString(L"pid")->Data() << "';";
				state = DataState::Edited; // TODO. return Added or Edited
			}
		}
		// https://stackoverflow.com/questions/5285456/android-sqlite-error-code-21

		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateProductImage(JsonObject ^ obj, bool deleteit)
	{
		//https://wiki.apache.org/couchdb/HTTP_Document_API#Attachments
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		if (deleteit)
		{
			osql << "DELETE " << PRODUCTMEDIA << " WHERE _id = " << STRCONV(obj->GetNamedString(L"_id"));
			state = DataState::Deleted;
		}
		else
		{
			const string id = STRCONV(obj->GetNamedString("_id"));
			const string rev = STRCONV(obj->GetNamedString("_rev"));
			const string pid = STRCONV(obj->GetNamedString("pid"));
			const bool istitleimage = obj->GetNamedBoolean("istitleimage");
			const string data = STRCONV(obj->GetNamedString("data"));
			const string ctype = STRCONV(obj->GetNamedString("contenttype"));
			const string title = STRCONV(obj->GetNamedString("title"));
			const string desc = STRCONV(obj->GetNamedString("desc"));

			// don't add relation if the category and product doesn't exist in the respective tables.
			if (HasData(PRODUCTMEDIA, "_id='" + id + "'"))
			{
				osql << "UPDATE " << PRODUCTMEDIA << " SET ";
				osql << "_rev = '" << rev << "', ";
				osql << "state = 0, ";
				osql << "pid = '" << pid << "', ";
				osql << "istitleimage = " << (istitleimage ? 1 : 0) << ", ";
				osql << "data = '" << data << "', ";
				osql << "contenttype = '" << ctype << "', ";
				osql << "title = '" << title << "', ";
				osql << "desc = '" << desc << "' ";
				osql << "where _id = '" << id << "'";
				state = DataState::Edited;
			}
			else
			{
				osql << "INSERT INTO " << PRODUCTMEDIA << " (_id, _rev, state, pid, istitleimage, data, contenttype, title, desc ) ";
				osql << "VALUES ('" << id << "', '" << rev << "', " << 0 << ", '" << pid << "', ";
				osql << (istitleimage ? 1 : 0) << ", '" << data << "', '" << ctype << "', '" << title << "', '" << desc << "')";
				state = DataState::Added;
			}
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateProductPricePlan(JsonObject ^ obj, bool deleteit)
	{
		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		if (deleteit)
		{
			if (IsSqlOk(BeginTrans()))
			{
				// delete all types of price plans
				osql << "DELETE FROM " << PRODUCTPRICESTDDETAILS << " WHERE pppid = '" << id << "';";
				osql << "DELETE FROM " << PRODUCTPRICINGTIERDETAILS << " WHERE pppid = '" << id << "';";
				osql << "DELETE FROM " << PRODUCTPRICEPLANS << " WHERE pppid = '" << id << "';";
				if (IsSqlOk(ExecSqlNative(osql.str())))
				{
					CommitTrans();
					state = DataState::Deleted;
					return state;
				}
				RollbackTrans();
			}
			throw CreateException(MessageIds::ReplicationIncomingDataError);
		}
		else
		{
			const string rev = STRCONV(obj->GetNamedString("_rev", ""));
			const string code = STRCONV(obj->GetNamedString("plancode", ""));
			const string desc = STRCONV(obj->GetNamedString("plandesc",""));
			
			//const JsonObject^ dateInit = obj->GetNamedObject("dateinit", nullptr);
			//const JsonObject^ dateEnded = obj->GetNamedObject("dateended", nullptr);

			const string dateinit =  STRCONV(obj->GetNamedString("dateinit", "0")->Data());
			const string dateended = STRCONV(obj->GetNamedValue("dateended")->ValueType != JsonValueType::Null ? obj->GetNamedString("dateended") : "0");

			if (HasData(PRODUCTPRICEPLANS, "_id='" + id + "'"))
			{
				osql << "UPDATE " << PRODUCTPRICEPLANS << " SET ";
				osql << "_rev = '" << rev << "', ";
				osql << "state = 0, ";
				osql << "plancode = '" << code << "', ";
				osql << "plandesc = '" << desc << "', ";
				osql << "dateinit = " << dateinit << ", ";
				osql << "dateended = " << dateended;
				osql << " where _id = '" << id << "';";
				state = DataState::Edited;
			}
			else
			{
				osql << "INSERT INTO " << PRODUCTPRICEPLANS ;
				osql << "( _id, _rev, state, plancode, plandesc, dateinit, dateended ) ";
				osql << " VALUES ";
				osql << "('" << id << "', '" << rev << "', 0, '" << code << "', '" << desc << "', " << dateinit << ", " << dateended << " );";
				state = DataState::Added;
			}
			if (!IsSqlOk(ExecSql(osql.str())))
			{
				//ThrowException(MessageIds::ReplicationIncomingDataError);
				state = DataState::Error;
			}
			return state;
		}
	}

	DataState SqliteDataManager::UpdateStandardPricePlanDetails(JsonObject ^ obj, bool deleteit)
	{
		if (deleteit) return DataState::Deleted;

		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string rev = STRCONV(obj->GetNamedString("_rev"));
		const string  ppid = STRCONV(obj->GetNamedString("pppid"));
		const double price = obj->GetNamedNumber("unitprice");

		if (HasData(PRODUCTPRICESTDDETAILS, "_id='" + id + "'"))
		{
			osql << "UPDATE " << PRODUCTPRICESTDDETAILS << " SET ";
			osql << "_rev = '" << rev << "', ";
			osql << "state = 0, ";
			osql << "pppid = '" << ppid << "', ";
			osql << "unitprice = " << price ;
			osql << " WHERE _id = '" << id << "';";
			state = DataState::Edited;
		}
		else
		{
			osql << "INSERT INTO " << PRODUCTPRICESTDDETAILS << " ( ";
			osql << "_id, _rev, state, pppid, unitprice ";
			osql << " ) VALUES ( '";
			osql << id << "', '" << rev << "', 0, '" << ppid << "', " << price;
			osql << " ); ";
			state = DataState::Added;
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}

	DataState SqliteDataManager::UpdateTieredPricePlanDetails(JsonObject ^ obj, bool deleteit)
	{
		if (deleteit) return DataState::Deleted;

		using namespace std;
		DataState state = DataState::Unmodified;
		ostringstream osql;
		const string id = STRCONV(obj->GetNamedString("_id"));
		const string rev = STRCONV(obj->GetNamedString("_rev"));
		const string  ppid = STRCONV(obj->GetNamedString("pppid"));
		const double price = obj->GetNamedNumber("unitprice");
		const long beginqty = (long)obj->GetNamedNumber("beginqty");
		const long endqty = (long)obj->GetNamedNumber("endqty");

		if (HasData(PRODUCTPRICINGTIERDETAILS, "_id='" + id + "'"))
		{
			osql << "UPDATE " << PRODUCTPRICINGTIERDETAILS << " SET ";
			osql << "_rev = '" << rev << "', ";
			osql << "state = 0, ";
			osql << "pppid = '" << ppid << "', ";
			osql << "unitprice = " << price << ", ";
			osql << "beginqty = " << beginqty << ", ";
			osql << "endqty = " << endqty;
			osql << " WHERE _id = '" << id << "';";
			state = DataState::Edited;
		}
		else
		{
			osql << "INSERT INTO " << PRODUCTPRICINGTIERDETAILS << " ( ";
			osql << "_id, _rev, state, pppid, unitprice, beginqty, endqty ";
			osql << " ) VALUES ( '";
			osql << id << "', '" << rev << "', 0, '" << ppid << "', " << price << ", " << beginqty << ", " << endqty ;
			osql << " ); ";
			state = DataState::Added;
		}
		if (!IsSqlOk(ExecSql(osql.str())))
		{
			//ThrowException(MessageIds::ReplicationIncomingDataError);
			state = DataState::Error;
		}
		return state;
	}



	/* SQL that list a single list of parent/child categories.
	ostr << R"(select a._id as catpcid, a.name as catpname, a.desc as catpdesc, c._id as catcid, c.name as catcname, c.desc as catcdesc
	from productcategories a inner join productcategorytocategoryrelations b on a._id = b.pcid
	inner join productcategories c on b.cid = c._id
	where a._id = )" << StringHelpers::Convert(pid) << "order by catpname, catcname;)";

	*/

	task<ProductCategoryVector^> SqliteDataManager::GetCategoryChildrenAsync(ProductCategory^ parent)
	{
		/*
			select a._id as catpcid, a.name as catpname, a.desc as catpdesc, c._id as catcid, c.name as catcname, c.desc as catcdesc
			from productcategories a inner join productcategorytocategoryrelations b on a._id = b.pcid
			inner join productcategories c on b.cid = c._id
			order by catpname, catcname;
		*/
		auto pid = parent ? parent->Id->Data() : L"rootcategory";
		auto t = create_task([this, pid]() -> ProductCategoryVector^
		{
			ostringstream ostr;
			ostr << "select a._id, a.name, a.desc, c.cidcount ";
			ostr << "from (select * from " << PRODUCTCATEGORIES << " where state < " << (int)DataState::Deleted << " order by name) a ";
			ostr << "inner join(select * from " << PRODUCTCATEGORYCATEGORYREL << " where state < " << (int)DataState::Deleted << ") b on a._id = b.cid ";
			ostr << "left join(select count(cid) as cidcount, pcid from " << PRODUCTCATEGORYCATEGORYREL << " where state < " << (int)DataState::Deleted << " group by pcid) c on c.pcid = b.cid ";
			ostr << "where b.pcid = '" << StringHelpers::Convert(pid) << "'";
			ProductCategoryVector^ list = ref new ProductCategoryVector();
			if (AcquireReadLock())
			{
				int ret = ExecSqlNative(
					ostr.str(),
					[list](void* p, int argc, char** argv, char** azColName)
				{
					if (argc == 4)
					{
						ProductCategory^ catChild = ref new ProductCategory(STRCONV(argv[0]));
						catChild->CanRaiseEvents = false;
						catChild->Name = STRCONV(argv[1]);
						catChild->Desc = STRCONV(argv[2]);
						catChild->HasChildren = argv[3] ? std::stoi(argv[3]) : 0; // atoi(argv[3]) > 0;
						list->Append(catChild);
						catChild->CanRaiseEvents = true;
					}
					return SQLITE_OK;
				}, nullptr);
				UnlockReadLock();
			}
			return list;
		});
		return t;
	}

	task<int> SqliteDataManager::GetCategoryChildrenCountAsync(ProductCategory ^ parent)
	{
		auto pid = parent ? parent->Id->Data() : L"rootcategory";
		auto t = create_task([this, pid]() -> int
		{
			wstringstream ostr;
			ostr << L"select count(cid) as cidcount from " << PRODUCTCATEGORYCATEGORYREL << " where pcid = '" << pid << "' and state < " << (int)DataState::Deleted << " group by pcid";
			if (AcquireReadLock())
			{
				int count = 0;
				int ret = ExecSqlNative(
					StringHelpers::Convert(ostr.str()),
					[&count](void* p, int argc, char** argv, char** azColName)
				{
					if (argc > 0)
					{
						count = argv[0] ? std::stoi(argv[0]) : 0;
					}
					return SQLITE_OK;
				});
				UnlockReadLock();
				return count;
			}
			else
			{
				return 0;
			}
		});
		return t;
	}

	task<bool> SqliteDataManager::SaveCategoryLocalAsync(ProductCategory ^ category, String^ pid)
	{
		return create_task([this, category, pid] 
		{
			// is category already available ?
			ostringstream ostr;
			ostr << "_id='" << StringHelpers::Convert(category->Id->Data()) << "'";
			if (this->CountRows(PRODUCTCATEGORIES, ostr.str())) // already exists. update
			{
				ostr.str("");
				ostr << "begin;";
				ostr << "update " << PRODUCTCATEGORIES << " set ";
				ostr << "name = '" << StringHelpers::Convert(category->Name->Data()) << "', ";
				ostr << "desc = '" << StringHelpers::Convert(category->Desc->Data()) << "', ";
				ostr << "state = " << (int)DataState::Edited;
				ostr << " where _id = '" << StringHelpers::Convert(category->Id->Data()) << "';";
				if (!StringHelpers::IsNullOrEmpty(pid))
				{
					ostr << "update "<< PRODUCTCATEGORYCATEGORYREL << " set state = " << (int)DataState::Deleted << " where cid = '" << StringHelpers::Convert(category->Id) << "';";
					ostr << "insert into " << PRODUCTCATEGORYCATEGORYREL << " (_id, _rev, cid, pcid, state) select '" << STRCONV(SysInfoHelpers::NewCouchDbId()) << "', '', '" << StringHelpers::Convert(category->Id) << "', '" << StringHelpers::Convert(pid) << "', " << (int)DataState::Added << " where (select Changes() > 0);";
				}
				ostr << "commit;";
			}
			else
			{
				ostr.str("");
				ostr << "begin;";
				ostr << "insert into " << PRODUCTCATEGORIES << " (_id, _rev, name, desc, state) values ('" << STRCONV(category->Id) << "', '', '" << STRCONV(category->Name) << "', '" << STRCONV(category->Desc) << "', " << (int)DataState::Added << " );";
				ostr << "insert into " << PRODUCTCATEGORYCATEGORYREL << " (_id, _rev, cid, pcid, state) select '" << STRCONV(SysInfoHelpers::NewCouchDbId()) << "', '', '" << StringHelpers::Convert(category->Id) << "', '" << StringHelpers::Convert(pid) << "', " << (int)DataState::Added << " where (select Changes() > 0);";
				ostr << "commit;";
			}
			int retCode = ExecSql(ostr.str());
			return retCode == 0; 
		});
	}

	task<bool> SqliteDataManager::DeleteCategoryAsync(String ^ id)
	{
		return create_task([this, id]
		{
			//TODO: WILL BE COMPLETED WHEN PRODUCTS ARE IN
			// first check if subcategories exists.
			// then check each subcategories for any products assigned to it
			// if products are in, then return false. nothing happens.
			// otherwise remove each subcategory by flagging state until this item is reached, provided no products assigned.

			ostringstream ostr;
			ostr << "update " << PRODUCTCATEGORYCATEGORYREL << " set state = " << (int)DataState::Deleted << " where cid = '" << STRCONV(id) << "' and pcid = '" << STRCONV(id) << "';";
			ostr << "update " << PRODUCTCATEGORYCATEGORYREL <<" set state = " << (int)DataState::Deleted << " where cid = '" << STRCONV(id) << "';";
			int ret = 1; // ExecSql(ostr.str());
			return ret == 0;
		});
	}

	//void SqliteDataManager::UpdateCategory(JsonObject ^ obj, bool isDeleted)
	//{
	//	// {"_id":"e628e869155203b67251fb6ec75f0cfc","_rev":"4-1397876cf47a82957e8b535aa1af9982","type":"category",
	//	// "name":"Furniture","parentid":null,"desc":"Furniture"}
	//	ostringstream osql;
	//	osql << "UPDATE " << CATEGORYTABLE << " SET ";
	//	osql << "Rev = '" << obj->GetNamedString("_rev")->Data() << ", ";
	//	osql << "Name = '" << obj->GetNamedString("name")->Data() << ", ";
	//	osql << "Desc = '" << obj->GetNamedString("desc")->Data() << " ";
	//	osql << "WHERE Id = '" << obj->GetNamedString("_id")->Data() << " ";
	//	osql << "AND Rev IS NOT '" << obj->GetNamedString("_rev")->Data() << ";";
	//	osql << "INSERT INTO " << CATEGORYTABLE << " ";
	//	osql << "( Id, Rev, Name, Desc ) ";
	//	osql << " VALUES ";
	//	osql << "( '" << obj->GetNamedString("_id")->Data() << "', '" << obj->GetNamedString("_rev")->Data() << "', '";
	//	osql << obj->GetNamedString("name")->Data() << "', " << obj->GetNamedString("desc")->Data() << "') ";
	//	osql << " WHERE (Select Changes() = 0); ";
	//	ExecSql(osql.str());
	//}



	int SqliteDataManager::CountRows(const std::string& table, const std::string& condition)
	{
		sqlite3ex* dbHandle = nullptr;
		try
		{
			std::ostringstream ostr;
			ostr << "select count(*) as CNT from " << table << " where " << condition;
			int count = 0;
			dbHandle = dbHandler.GetDbHandle();
			auto result = sqlite3_exec(dbHandle->dbHandle, ostr.str().c_str(), [](void* p, int argc, char** argv, char** azColName)
			{
				//int& cnt = (int)(*(int*)(p));
				(int)(*(int*)(p)) = strtol(argv[0], nullptr, 10);
				return SQLITE_OK;
			}, &count, nullptr);
			dbHandler.CloseDb(dbHandle);
			return count;
		}
		catch (...)
		{
			OutputDebugString(L"CountRows method failed to execute.");
			dbHandler.CloseDb(dbHandle);
			return 0;
		}
	}

	bool SqliteDataManager::HasData(const std::string & table, const std::string & condition)
	{
		ostringstream osql;
		osql << "select exists(select 1 from " << table << " where " << condition << " limit 1);";
		int bit = 0;
		auto dbHandle = dbHandler.GetDbHandle();
		auto result = sqlite3_exec(dbHandle->dbHandle, osql.str().c_str(), [](void* p, int argc, char** argv, char** azColName)
		{
			(int)(*(int*)(p)) = strtol(argv[0], nullptr, 10);
			return SQLITE_OK;
		}, &bit, nullptr);
		dbHandler.CloseDb(dbHandle);
		return bit == 1;
	}

	string SqliteDataManager::GetRowKey(const std::string & table, const std::string & condition)
	{
		ostringstream osql;
		osql << "select _id from " << table << " where " << condition << " limit 1;";
		string retId;
		auto result = ExecSqlNative(osql.str().c_str(), [&retId](void* p, int argc, char** argv, char** azColName)
		{
			auto str = argv[0];
			retId = str;
			return SQLITE_OK;
		});
		return retId;
	}

	int SqliteDataManager::LastSqlChanges()
	{
		return sqlite3_changes(dbHandler.GetCurrentHandle()->dbHandle);
	}

	int SqliteDataManager::ExecSql(const std::wstring & sql, std::function<void(const CDBROW&)> cb)
	{
		auto ssql = StringHelpers::Convert(sql);
		return ExecSql(ssql, cb);
	}

	int SqliteDataManager::ExecSql(const std::string & sql, std::function<void(const CDBROW&)> cb)
	{
		sqlite3ex* dbHandle = nullptr;
		try
		{
			OutputDebugStringA("\n--------------------\n");
			OutputDebugStringA(sql.c_str());
			OutputDebugStringA("\n--------------------\n");
			char* errMsg = 0;
			int result;
			dbHandle = dbHandler.OpenDb();
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
				OutputDebugStringA("\n");
				sqlite3_free(errMsg);
			}
			dbHandler.CloseDb(dbHandle);
			return result;
		}
		catch (...)
		{
			OutputDebugString(L"ExecSql method failed.");
			dbHandler.CloseDb(dbHandle);
			return SQLITE_ABORT;
		}
	}

	int SqliteDataManager::ExecSqlNative(const std::wstring& sql, SqliteCallback cb, void* state, bool intransaction)
	{
		auto ssql = StringHelpers::Convert(sql);
		return ExecSqlNative(ssql, cb, state, intransaction);
	}

	int SqliteDataManager::ExecSqlNative(const std::string& sql, SqliteCallback cb, void* state, bool intransaction)
	{
		if (sql.empty()) return SQLITE_OK;

		sqlite3ex* dbHandle = nullptr;

		try
		{
			OutputDebugStringA("\n--------------------\n");
			OutputDebugStringA(sql.c_str());
			OutputDebugStringA("\n--------------------\n");

			int result;


			dbHandle = dbHandler.OpenDb();


			if (intransaction)
			{
				result = BeginTrans();
				if (!IsSqlOk(result))
					return result;
			}

			char* errMsg = 0;
			if (cb)
			{
				result = sqlite3_exec(
					dbHandle->dbHandle,
					sql.c_str(),
					[](void* p, int argc, char** argv, char** azColName)
				{
					return (*static_cast<SqliteCallback*>(p))(nullptr, argc, argv, azColName);
				},
					&cb,
					&errMsg
					);
			}
			else
			{
				result = sqlite3_exec(dbHandle->dbHandle, sql.c_str(), nullptr, nullptr, &errMsg);
			}

			// SQLITE_ABORT is what we are using to allow the lamda to exit
			// when we don't want all recoreds to be processed. So it is not
			// considered an exception
			if (result != SQLITE_ABORT && result != SQLITE_OK)
			{


				OutputDebugStringA(errMsg);
				
#ifdef _DEBUG
				String^ thrownMsg = StringHelpers::ConvertToString(errMsg);
#endif

				sqlite3_free(errMsg);


				if (intransaction)
					RollbackTrans();

				dbHandler.CloseDb(dbHandle);

#ifdef _DEBUG
				throw Exception::CreateException((DWORD)result, thrownMsg);
#endif

				return result;
			}

			if (intransaction)
				result = CommitTrans();

			dbHandler.CloseDb(dbHandle);

			return result;
		}
		catch (...)
		{
			OutputDebugString(L"ExecSql method failed.");

			dbHandler.CloseDb(dbHandle);

			if (intransaction)
				RollbackTrans();

			return SQLITE_ABORT;
		}
	}

	int SqliteDataManager::BeginTrans()
	{
#if DISABLETRANSACTIONS == 0
		auto dbhandle = dbHandler.OpenDb();
		if (dbhandle->transactionCount == 0)
		{
			int result;
			char* errMsg = 0;
			result = sqlite3_exec(dbhandle->dbHandle, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
			if (result != SQLITE_OK)
			{
				OutputDebugStringA(errMsg);
				sqlite3_free(errMsg);
				return result;
			}
		}
		dbhandle->transactionCount++;
		if (dbhandle->transactionCount > 1)
			OutputDebugStringA("\nUsing Existing Transaction\n");
		else
			OutputDebugStringA("\nBegan Transaction\n");
#endif
		return SQLITE_OK;
	}

	int SqliteDataManager::CommitTrans()
	{
#if DISABLETRANSACTIONS == 0
		auto dbhandle = dbHandler.GetCurrentHandle();
		dbhandle->transactionCount--;
		if (dbhandle->transactionCount <= 0)
		{
			int result;
			char* errMsg = 0;
			result = sqlite3_exec(dbhandle->dbHandle, "COMMIT TRANSACTION;", nullptr, nullptr, &errMsg);
			if (result != SQLITE_OK)
			{
				OutputDebugStringA(errMsg);
				sqlite3_free(errMsg);
				return result;
			}
		}
		if (dbhandle->transactionCount == 0)
			OutputDebugStringA("\nCommitted Transaction\n");
		else
			OutputDebugStringA("\nInner transaction transaction - commit point hasn't reached.\n");
		dbHandler.CloseDb(dbhandle);
#endif
		return SQLITE_OK;
	}

	int SqliteDataManager::RollbackTrans()
	{
#if DISABLETRANSACTIONS == 0
		auto dbhandle = dbHandler.GetCurrentHandle();
		dbhandle->transactionCount--;
		if (dbhandle->transactionCount <= 0)
		{
			int result;
			char* errMsg = 0;
			result = sqlite3_exec(dbhandle->dbHandle, "ROLLBACK TRANSACTION;", nullptr, nullptr, &errMsg);
			if (result != SQLITE_OK)
			{
				OutputDebugStringA(errMsg);
				sqlite3_free(errMsg);
				return result;
			}
		}
		if (dbhandle->transactionCount == 0)
			OutputDebugStringA("\nRolled back.\n");
		else
			OutputDebugStringA("\nInner transaction transaction - rollback point hasn't reached.\n");
		dbHandler.CloseDb(dbhandle);
#endif
		return SQLITE_OK;
	}

	bool SqliteDataManager::IsSqlOk(int result)
	{
		if (result != SQLITE_OK)
		{
			OutputDebugStringA(GetSqliteError(result));
			return false;
		}
		return true;
	}

	bool SqliteDataManager::AcquireWriteLock()
	{
		//dbHandler.cs.lock();
		return true; // queryLock.try_lock();
	}

	bool SqliteDataManager::AcquireReadLock()
	{
		//dbHandler.cs.lock();
		return true; // queryLock.try_lock_shared();
	}

	void SqliteDataManager::UnlockWriteLock()
	{
		//dbHandler.cs.unlock();
		//queryLock.unlock();
	}

	void SqliteDataManager::UnlockReadLock()
	{
		//queryLock.unlock_shared();
		//dbHandler.cs.unlock();
	}


	/************Sqlite callback function ****************/

	int _stdcall sqliteExecCallback(void* p, int argc, char** argv, char** azColName)
	{
		if (argc)
		{
			CDBROW rowData(argc);
			for (int i = 0; i < argc; i++)
			{
				std::string colName = azColName[i];
				std::string colVal;
				if (argv[i])
					colVal = argv[i];
				rowData.insert(make_pair(colName, colVal));
			}
			(*((std::function<void(const CDBROW&)>*)(p)))(rowData);
		}
		return SQLITE_OK;
	}

}