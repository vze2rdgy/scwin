#pragma once

#include <atomic>
#include <shared_mutex>
#include <string>

#include "SqliteHandleManager.h"

#include "dto.h"
#include "ProductCategory.h"
#include "Product.h"
#include "ProductFamily.h"
#include "ProductImage.h"
#include "ProductInventory.h"
#include "ProductInventoryLocation.h"
#include "ProductAttribute.h"
#include <boost/graph/undirected_graph.hpp>

using namespace std;
using namespace Windows::Foundation::Collections;

namespace scwin
{

//#define USERROLETABLE	"userroles"
//#define USERTABLE		"users"

//#define USERRIGHTS					"userrights"
#define APPSETTINGS					"appsettings"
#define DOCUMENTKEYS				"dockeys"
#define ADDRESSES					"address"
#define ORDERSTATUS					"orderstatus"
#define PAYMENTSTATUS				"paymentstatus"
#define PAYMENTINSTRUMENT			"paymentinstrument"
#define CUSTOMERORGS				"customersorg"
#define CORGCOSTCENTER				"corgcostcenter"
#define CUSTOMERORGCONTACTS			"customersorgcontact"
#define FIELDREP					"fieldrep"

#define PRODUCTCATEGORIES			"productcategory"
#define PRODUCTCATEGORYCATEGORYREL	"productcategorytocategoryrelation"
#define PRODUCTTABLE				"product"
#define CATEGORYTOPRODUCT			"categorytoproduct"
#define PRODUCTFAMILIES				"productfamily"
#define PRODUCTATTRIBUTE			"productattribute"	// master table of attributes
#define PRODUCTATTRIBUTEVALUE		"productattributevalue"	// relation between attributes and product
#define PRODUCTPRICEPLANS			"productpriceplan"	//pricing plans for a product
#define PRODUCTPRICESTDDETAILS		"priceplanstddetail"	//pricing plans for a product
#define PRODUCTPRICINGTIERDETAILS	"priceplantierdetail"
#define PRODUCTCOMPOSITION			"productcomposition"
#define INVENTORYLOCATION			"inventorylocation"
#define ACTIVEINVENTORY				"activeinventory"
#define INVENTORYPROPERTY			"inventoryproperty"
#define INVENTORYTRX				"inventorytrx"
#define PRODUCTMEDIA				"productmedia"
#define INVENTORYMOVEMENTTYPE		"inventorymovementtypes"
#define SALESORDERS					"salesorder"
#define SALESORDERDETAILS			"salesorderdetails"	
#define SALESORDERSTATUSHISTORY		"salesorderstatushistory"
#define SALESORDERITEMSTATUSHISTORY	"salesorderitemstatushistory"
#define SALESORDERPAYMENTHISTORY	"salesorderpaymenthistory"
#define APPBUSINESSORG				"appbusinessorg"


#define STRSAFECONV StringHelpers::SqlSafeConvert
#define STRCONV StringHelpers::Convert


	const string excludedTables = " ('appsettings', 'orderstatus', 'inventorymovementtypes', 'paymentstatus', 'paymentinstrument');";

	typedef std::function<bool(const std::string& colName, const std::string& colValue, bool islast)> LightCallback;
	typedef boost::undirected_graph<ProductCategory^> CategoryGraph;

	template <typename A>
	struct IsCouchDbObjectLess
	{
		bool operator()(A a, A b) const
		{
			return a->Id < b->Id;
		}
	};

	template <typename A>
	struct IsCouchDbObjectEqual
	{
		bool operator()(A a, A b) const
		{
			return a->Id == b->Id;
		}
	};

	typedef Vector<ProductCategory^, IsCouchDbObjectEqual<ProductCategory^>> ProductCategoryVector;
	typedef std::multimap<String^, String^> PRODUCTTYPEIDMAP;


	class SqliteDataManager
	{
		// store current user profile
		UserContext^ currentUser;

		SqliteHandleManager dbHandler;

		// Below mutex this will suspend remote data fetching while a local query is in progress. 
		// that way we can avoid any data that will be missed when a simultaneous querying and data update is in progress
		// but we will have to find more details on how Sqlite locks a table a query and table updates are concurrentlhy 
		// in progress. Read: https://sqlite.org/isolation.html
		std::shared_mutex queryLock;


	public:
		volatile atomic_bool replicationInProgress;

	private:


		////////////////////// TABLE CREATION INTERNAL METHODS //////////////////////////
		// Create a default table with an Id and TEXT fields.
		bool CreateSystemTables();

		bool CreateCommonTables();

		// Create the products related tables sqlite table.
		bool CreateProductRelatedTables();

		bool CreateInventoryTables();

		// create all sales order related tables
		bool CreateSalesOrderRelatedTable();
		
		// create customer and sale men related tables
		bool CreateCustomerRelatedTables();
		
		bool CreatePersonnelRelatedTables();

		// prepopulated data. these are for table which are used as lookups and prepared master records
		// such as order status, payment status, etc.
		int PrepopulateMasterTableData();

		//VVVVVVVVVVVVVVVVVV REPLICATOR USES THESE METHODS VVVVVVVVVVVVVVVVVVVVVVVVVVV

		// Common Delete function for replicator
		int DeleteLocalItems(std::initializer_list<string> tables);

		// Below methods looks for local changes and publishes them to remote couchdb database.

		// Obtain a remote couchdb object using its identifier.
		JsonObject^ GetRemoteObject(String^ documentId);

		void GetCommonDataLocalChanges(JsonArray^ fill);
		void GetProdCategoryLocalChanges(JsonArray^ fill);
		void GetProductLocalChanges(JsonArray^ fill);
		void GetInventoryLocalChanges(JsonArray^ fill);
		void GetOrdersLocalChanges(JsonArray^ fill);

		// Below are for fetching customer data updates locally made and send it to coucdhb
		void GetCustomerLocalChanges(JsonArray^ fill);


		////// Below methods pull remote data and stores them in the local cache only when
		////// local cache doesn't contain them.
		// Updates a product cartegory object.
		// Update can be made by the replicator (json object from remote
		// service is converted to category object using ProductDataBuilder static class),
		// or by the user interface directly to the table.

		DataState UpdateAppSettings(JsonObject^ obj);
		DataState UpdateEnumTable(String^ tableName, JsonObject^ obj);
		DataState UpdateAddress(JsonObject^ obj, bool deleteit);

		DataState UpdateCategory(JsonObject^ obj, bool deleteit);
		DataState UpdateCategoryToCategoryRelations(JsonObject^ dataobj, bool deleteit);
		DataState UpdateProduct(JsonObject^ obj, bool deleteit);
		DataState UpdateCategoryToProduct(JsonObject^ obj, bool deleteit);
		DataState UpdateProductImage(JsonObject^ obj, bool deleteit);


		DataState UpdateProductPricePlan(JsonObject^ obj, bool deleteit);
		DataState UpdateStandardPricePlanDetails(JsonObject^ obj, bool deleteit);
		DataState UpdateTieredPricePlanDetails(JsonObject^ obj, bool deleteit);

		DataState UpdateProductFamily(JsonObject^ obj, bool deleteit);
		DataState UpdateProductAttribute(JsonObject^ obj, bool deleteit);
		DataState UpdateProductAttributeValue(JsonObject^ obj, bool deleteit);

		DataState UpdateInventoryLocation(JsonObject^ obj);
		DataState UpdateActiveInventory(JsonObject^ obj);
		DataState UpdateInventoryProperty(JsonObject^ obj);
		DataState UpdateInventoryTrx(JsonObject^ obj);

		DataState UpdateCustomerOrg(JsonObject^ obj, bool deleteid);
		DataState UpdateCOrgCostCenter(JsonObject^ obj, bool deleteid);
		DataState UpdateCustomerOrgContact(JsonObject^ obj, bool deleteid);
		DataState UpdateFieldReps(JsonObject^ obj, bool deleteid);

		DataState UpdateSalesOrder(JsonObject^ obj);
		DataState UpdateSalesOrderDetails(JsonObject^ obj);
		DataState UpdateSalesOrderStatusHistory(JsonObject^ obj);
		DataState UpdateSalesOrderStatusItemHistory(JsonObject^ obj);
		DataState UpdateSalesOrderPaymentHistory(JsonObject^ obj);

		DataState UpdateAppBusinessOrg(JsonObject^ obj);

		const char* GetSqliteError(int result);


		//^^^^^^^^^^^^^^^^^^^^^^^^^ REPLICATOR USES THESE METHODS ^^^^^^^^^^^^^^^^^^^^^^^^

	public:
		SqliteDataManager();
		~SqliteDataManager();

		UserContext^ getUser() { return currentUser; }

		void setUser(UserContext^ user) { currentUser = user; }

		std::string GetSettingsValue(const std::string& key)
		{
			std::stringstream osql;
			osql << "propname = '" << key << "'";
			if (!CountRows("appsettings", osql.str()))
			{
				return "";
			}

			osql.str("");
			osql << "SELECT propvalue FROM appsettings WHERE propname = '" << key << "'";
			std::string output;
			if (!IsSqlOk(ExecSql(osql.str(), [this, &output, &key](const CDBROW& row)
			{
				auto it = row.find("propvalue");
				if (it != row.end())
				{
					output = it->second;
				}
				else
				{
					output = "";
				}
			})))
			{
				throw std::runtime_error("Cache db exception");
			}
			return output;
		}

		template<typename TVal>
		void SetSettingsValue(const std::string& key, const TVal& val, bool isRoaming, bool unquotedvalue = false)
		{
			std::ostringstream osql;
			osql << "UPDATE appsettings SET ";
			if (unquotedvalue)
				osql << "propvalue = " << val << ", ";
			else
				osql << "propvalue = '" << val << "', ";
			osql << "isroaming = " << isRoaming << ", state = " << (int)DataState::Edited << "  WHERE propname = '" << key << "'; ";
			osql << "INSERT INTO appsettings (propname, propvalue, isroaming, state) ";
			if (unquotedvalue)
				osql << "SELECT '" << key << "', " << val << ", " << isRoaming << ", " << (int)DataState::Added;
			else
				osql << "SELECT '" << key << "', '" << val << "', " << isRoaming << ", " << (int)DataState::Added;
			osql << " WHERE (Select Changes() = 0); ";
			auto ret = ExecSql(osql.str());
			if (!IsSqlOk(ret))
			{
				throw std::runtime_error("Cache db exception.");
			}
		}

		// see documentation https://www.sqlite.org/lang_vacuum.html
		//https://www.sqlite.org/pragma.html#pragma_auto_vacuum
		bool VacuumDb();
		bool Encrypt(const std::string& salt);
		bool Authenticate(const std::string& userId, const std::string& pwd);

		/// <summary> When user supplies a condition with key-value pair as TABLEFIELD-FIELDVALUE
		/// that map is converted to a string suitable to use with where clause
		/// </summary>
		std::string ConditionsAsWhereClause(const std::string& tablename, const CONDITIONS& cont, const std::string& combiner = "and");


		// Removes all tables and indices from the database.
		bool ClearDb();
		
		bool CreateDataSchema();

		SqliteHandleManager& GetDbHandler() { return this->dbHandler; }

		// special couchdb methods
		string GetLastRevisionNumber(const string& id);


		int CountRows(const std::string& table, const std::string& condition);
		bool HasData(const std::string& table, const std::string& condition);
		string GetRowKey(const std::string& table, const std::string& condition);
		int LastSqlChanges();
		
		int ExecSql(const std::string& sql, std::function<void(const CDBROW&)> cb = nullptr);
		int ExecSql(const std::wstring& sql, std::function<void(const CDBROW&)> cb = nullptr);
		int ExecSqlNative(const std::wstring& sql, SqliteCallback cb = nullptr, void* state = nullptr, bool intransaction = false);
		int ExecSqlNative(const std::string& sql, SqliteCallback cb = nullptr, void* state = nullptr, bool intransaction = false);

		int BeginTrans();
		int CommitTrans();
		int RollbackTrans();
		
		bool IsSqlOk(int result);

		bool AcquireWriteLock();

		bool AcquireReadLock();

		void UnlockWriteLock();

		void UnlockReadLock();

		bool DropTable(const std::string& tableName);




		/////////////////// REPLICATOR METHODS /////////////////////////////

		// called by replicator to pull all the local changes and submit to the remote database
		task<JsonArray^> FetchLocalChangesAsync();
		JsonArray^ FetchLocalChanges();

		// called by replicator to transfer all new or modified items from the remote database to the local cache.
		void SubmitData(JsonObject^ jobj);
		void SubmitData(JsonArray^ rows);

		// called by the replicator to remove all server deleted items from the local cache.
		void PurgeDeletedItems(const std::unordered_set<String^>& deleted);

		int UpdateRowRevisionNumber(const string& id, const string& rev, const string& table);

		//////////////////// APPLICATION DATA ACCESS METHODS /////////////////////

		// Reads a list of categories satisfying the conditions (key-value pairs).
		// If condition is empty, all are returned.
		// The categories are loaded in a hierarchical fashion.
		task<ProductCategoryVector^> GetCategoryChildrenAsync(ProductCategory^ parent);
		// get how many children categories are available for the parent category.
		task<int> GetCategoryChildrenCountAsync(ProductCategory^ parent);
		// Save product category. pid is not required
		// when saving an existing category, unless a move situation (TODO). 
		// For a new category, if the parent pid is null, 
		// it will be saved at root level
		task<bool> SaveCategoryLocalAsync(ProductCategory^ category, String^ pid);
		// Deletes a category object.
		task<bool> DeleteCategoryAsync(String^ id);



	};
}