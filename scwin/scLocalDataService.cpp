#include "pch.h"
#include "scLocalDataService.h"
#include "scRemoteDataService.h"
#include "scMainViewModel.h"
#include "boost/algorithm/string.hpp"
#include "../scshared/CodeGenerator.h"

using namespace std;
using namespace scwin;
using namespace scwin::services;
using namespace std::placeholders;

scLocalDataService* scLocalDataService::singleton = nullptr;

#define DBM this->GetCache()
#define ISDBOK this->GetCache().IsSqlOk
#define DBFAILED !this->GetCache().IsSqlOk

#define DBSELECTOR(sql) DBM.ExecSqlNative(sql, [=](void* p, int argc, char** argv, char** azColName)
#define DBSELECTORP(sql, prm) DBM.ExecSqlNative(sql, [this, prm](void* p, int argc, char** argv, char** azColName)
#define DBSELECTORALLP(sql) DBM.ExecSqlNative(sql, [=](void* p, int argc, char** argv, char** azColName)

scLocalDataService::scLocalDataService()
{
	rmData = new scRemoteDataService();
	replicator = new ReplicatorService(rmData);
	replicator->Subscribe([this](const std::wstring& typeName, const std::wstring& json)->void
	{
		this->ReplicationCallback(typeName, json);
	});

}

void scwin::scLocalDataService::ReplicationCallback(const std::wstring & typeName, const std::wstring & jsonData)
{
	if (typeName == L"UserRoles")
	{

	}
}

void scwin::scLocalDataService::LogEx(Exception ^ ex)
{
	OutputDebugStringA(((string)STRCONV(ex->Message)).c_str());
}


scLocalDataService::~scLocalDataService()
{
	mainVm = nullptr;
	delete replicator;
	delete rmData;
}


scLocalDataService* scLocalDataService::Instance()
{
	if (!singleton)
	{
		GUARDEDGLOCK;
		if (!singleton)
		{
			singleton = new scLocalDataService();
		}
	}
	return singleton;
}

void scwin::scLocalDataService::Cleanup()
{
	if (singleton)
	{
		GUARDEDGLOCK;
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}
}


bool scLocalDataService::Connect(DependencyObject^ mainVm)
{
	this->mainVm = mainVm;
	return rmData->Connect();
}

bool scLocalDataService::Disconnect()
{
	this->mainVm = nullptr;
	return rmData->Disconnect();
}

SqliteDataManager & scwin::scLocalDataService::GetCache()
{
	return replicator->GetDbMgr();
}

bool scwin::scLocalDataService::StartTransaction()
{
	// open a dbhandle
	return DBM.IsSqlOk(DBM.BeginTrans());
}

bool scwin::scLocalDataService::CommitTransaction()
{
	return DBM.IsSqlOk(DBM.CommitTrans());
}

void scwin::scLocalDataService::CancelTransaction()
{
	DBM.RollbackTrans();
}

AppProperty ^ scwin::scLocalDataService::GetProperty(String ^ name)
{
	if (!replicator)
	{
		return nullptr;
	}
	auto locName = StringHelpers::Convert(name->Data());
	auto retVal = StringHelpers::Convert(replicator->GetSettingsValue(locName));
	return ref new AppProperty(name, ref new String(retVal.c_str()));
}

IAsyncOperation<AppProperty^>^ scwin::scLocalDataService::GetPropertyAsync(String ^ name)
{
	return create_async([=]()
	{
		return GetProperty(name);
	});
}

void scwin::scLocalDataService::SetProperty(AppProperty ^ prop)
{
	if (!replicator) // don't do anything
	{
		return;
	}
	replicator->SetSettingsValue<std::string>(StringHelpers::Convert(prop->Name->Data()), StringHelpers::Convert(prop->Value->ToString()->Data()), prop->IsRoaming);
}

task<void> scwin::scLocalDataService::SetPropertyAsync(AppProperty ^ prop)
{
	return create_task([=]() -> void
	{
		SetProperty(prop);
	});
}

String^ scwin::scLocalDataService::AutoGenerateCode(String ^ tableName)
{
	String^ tableCode = String::Concat(tableName, "_CodeField");
	string numbuckets = DBM.GetSettingsValue(STRCONV(tableCode));

	string newlyEncoded;
	string newCode = scshared::CodeGenerator::GetNextOrderNumber(numbuckets, STRCONV(GetCurrentUser()->Name), &newlyEncoded);

//	// There are five buckets with each bucket value between 0 to 25 representing an alpha character.
//	// alternate numbers are counted in reverse.
//	const char* alphaArray[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
//	int bucketCount = 5;
//	if (numbuckets.empty())
//		numbuckets = "0,0,0,0,-1";
//	vector<string> buckets = StringHelpers::Split(numbuckets, ",");
//
//	// if all bucket memers are already 25, then insert into bucket item at the top
//	// and set all to zero
//	bool isoverflowing = false;
//
//BUCKETSTEPS:
//
//	for (int i = buckets.size() - 1; i >= 0; i--)
//	{
//		auto val = buckets[i];
//		auto ival = std::strtol(val.c_str(), nullptr, 10);
//		if (ival == 25)
//		{
//			if (i == 0)
//			{
//				isoverflowing = true;
//				break;
//			}
//			else
//			{
//				// increment prior bucket and set this bucket to 0
//				buckets[i - 1] = std::strtol(buckets[i - 1].c_str(), nullptr, 10) + 1;
//				buckets[i] = "0";
//				break;
//			}
//		}
//		else
//		{
//			// every altenate value char is revearse alpha
//			// ie. first is 0-25, next is 25-0, and so on.
//			// we are only bothered with one bucket. Above
//			// continue statement will ensure that
//			// bucket member is shifted left when current
//			// bucket member has "overflown"
//			buckets[i] = std::to_string(++ival);
//			break;
//		}
//	}
//
//	if (isoverflowing)
//	{
//		buckets.insert(buckets.cbegin(), "0");
//		for (int i = 1; i < buckets.size() - 1; i++)
//			buckets[i] = "0";
//		buckets[buckets.size() - 1] = "-1";
//
//		bucketCount = buckets.size();
//
//		goto BUCKETSTEPS;
//	}
//
//	String^ output = "";
//	string joined;
//	for (int i = 0; i < buckets.size(); i++)
//	{
//		auto val = buckets[i];
//		auto ival = std::strtol(val.c_str(), nullptr, 10);
//		if (0 == (i % 2))
//		{
//			output += StringHelpers::ConvertToString(alphaArray[ival]);
//		}
//		else
//		{
//			output += StringHelpers::ConvertToString(alphaArray[25 - ival]);
//		}
//		joined += val;
//		if (i < buckets.size() - 1)
//			joined += ",";
//	}
//
//	output = String::Concat(output, "-");
//	output = String::Concat(output, StringHelpers::ConvertToString(DBM.GetSettingsValue("usercrccode").c_str()));
	//DBM.SetSettingsValue(STRCONV(tableCode), joined, true);
	//return output;

	DBM.SetSettingsValue(STRCONV(tableCode), newlyEncoded, true);
	return StringHelpers::ConvertToString(newCode.c_str());

}

IAsyncOperation<UserContext^>^ scwin::scLocalDataService::VerifySession()
{
	return create_async([this](cancellation_token token) -> UserContext^
	{
		auto uc = this->rmData->VerifySession().get();
		if (uc)
		{
			auto ap = this->replicator->Begin(uc, true);
			scMainViewModel^ vm = safe_cast<scMainViewModel^>(this->mainVm);
			if (vm)
			{
				vm->DisplayProgress = Visibility::Visible;
				vm->SetStatusMessage(L"Please wait while data is being cached from the remote server...");
				vm->ProgressBarProgress = 5;
			}
			ap->Progress = ref new AsyncOperationProgressHandler<ReplicationStatus, int>([vm]
			(IAsyncOperationWithProgress<ReplicationStatus, int>^ progress, int progressInfo)
			{
				if (vm)
				{
					vm->ProgressBarProgress = (double)progressInfo;
				}
			});
			ap->Completed = ref new AsyncOperationWithProgressCompletedHandler<ReplicationStatus, int>([vm]
			(IAsyncOperationWithProgress<ReplicationStatus, int>^ progress, AsyncStatus status)
			{
				if (vm)
				{
					vm->ProgressBarProgress = 100;
					vm->DisplayProgress = Visibility::Collapsed;
					vm->ClearMessages();
				}
			});
		}
		return uc;
	});
}

IAsyncOperation<UserContext^>^ scwin::scLocalDataService::Signup(String ^ email, String ^ pwd, String ^ company, String^ fullName)
{
	return create_async([this, email, pwd, company, fullName](cancellation_token token) -> UserContext^
	{
		auto uc = rmData->Signup(email, pwd, company, fullName).get();
		if (uc)
		{
			auto ap = this->replicator->Begin(uc, false);
			scMainViewModel^ vm = safe_cast<scMainViewModel^>(this->mainVm);
			if (vm)
			{
				vm->DisplayProgress = Visibility::Visible;
				vm->SetStatusMessage(L"Please wait while data is being cached from the remote server...");
				vm->ProgressBarProgress = 5;
			}
			ap->Progress = ref new AsyncOperationProgressHandler<ReplicationStatus, int>([vm]
			(IAsyncOperationWithProgress<ReplicationStatus, int>^ progress, int progressInfo)
			{
				if (vm)
				{
					vm->ProgressBarProgress = (double)progressInfo;
				}
			});
			ap->Completed = ref new AsyncOperationWithProgressCompletedHandler<ReplicationStatus, int>([vm]
			(IAsyncOperationWithProgress<ReplicationStatus, int>^, AsyncStatus status)
			{
				if (vm)
				{
					vm->ProgressBarProgress = 100;
					vm->DisplayProgress = Visibility::Collapsed;
					vm->ClearMessages();
				}
			});
		}
		return uc;
	});
}

IAsyncOperation<UserContext^>^ scwin::scLocalDataService::Signin(String ^ email, String ^ pwd)
{
	return create_async([this, email, pwd](cancellation_token token) -> UserContext^
	{
		try
		{
			auto uc = rmData->Signin(email, pwd).get();
			if (uc)
			{
				auto ap = this->replicator->Begin(uc, true);
				scMainViewModel^ vm = safe_cast<scMainViewModel^>(this->mainVm);
				if (vm)
				{
					vm->DisplayProgress = Visibility::Visible;
					vm->SetStatusMessage(L"Please wait while data is being cached from the remote server...");
					vm->ProgressBarProgress = 5;
				}
				ap->Progress = ref new AsyncOperationProgressHandler<ReplicationStatus, int>([vm]
				(IAsyncOperationWithProgress<ReplicationStatus, int>^ progress, int progressInfo)
				{
					if (vm)
					{
						vm->ProgressBarProgress = (double)progressInfo;
					}
				});
				ap->Completed = ref new AsyncOperationWithProgressCompletedHandler<ReplicationStatus, int>([vm]
				(IAsyncOperationWithProgress<ReplicationStatus, int>^, AsyncStatus status)
				{
					if (vm)
					{
						vm->ProgressBarProgress = 100;
						vm->DisplayProgress = Visibility::Collapsed;
						vm->ClearMessages();
					}
				});
			}
			return uc;
		}
		catch (Exception^ ex)
		{
			throw ex;
		}
	});
}

IAsyncOperation<bool>^ scwin::scLocalDataService::SignOut()
{
	auto ap = this->replicator->End();
	ap->Progress = ref new AsyncOperationProgressHandler<ReplicationStatus, int>([]
	(IAsyncOperationWithProgress<ReplicationStatus, int>^ progress, int progressInfo)
	{

	});
	ap->Completed = ref new AsyncOperationWithProgressCompletedHandler<ReplicationStatus, int>([]
	(IAsyncOperationWithProgress<ReplicationStatus, int>^, AsyncStatus status)
	{

	});
	return rmData->SignOut();
}

UserContext ^ scwin::scLocalDataService::GetCurrentUser()
{
	return DBM.getUser();
}

IAsyncOperation<IVector<UserRole^>^>^ scwin::scLocalDataService::ListRoles()
{
	return create_async([this](cancellation_token canceltoken)-> IVector<UserRole^>^
	{
		return this->rmData->ListRolesAsync().get();
	});
}

IAsyncOperation<bool>^ scwin::scLocalDataService::UpsertRole(UserRole ^ role)
{
	return create_async([this, role](cancellation_token canceltoken)->bool
	{
		return rmData->UpsertRole(role).get();
	});
}

IAsyncOperation<bool>^ scwin::scLocalDataService::RemRole(String ^ roleName)
{
	return rmData->RemRole(roleName);
}

task<MessageIds> scwin::scLocalDataService::ResetUserPassword(String ^ userId)
{
	return rmData->ResetUserPassword(userId);
}

IAsyncOperation<IUserProfileList^>^ scwin::scLocalDataService::ListUsers()
{
	return create_async([this](cancellation_token canceltoken)->IUserProfileList^
	{
		return this->rmData->ListUsersAsync().get();
	});
}

IAsyncOperation<bool>^ scwin::scLocalDataService::UpsertUser(UserProfile ^ role, String^ password)
{
	return create_async([this, role, password](cancellation_token canceltoken)->bool
	{
		return rmData->UpsertUser(role, password).get();
	});

}

IAsyncOperation<bool>^ scwin::scLocalDataService::RemUser(String ^ emailAddress)
{
	return rmData->RemUser(emailAddress);
}

IAsyncOperation<UserProfile^>^ scwin::scLocalDataService::GetUser(String ^ userid)
{
	throw ref new NotImplementedException();
}

IAsyncOperation<IUserProfileList^>^ scwin::scLocalDataService::ListUsersByRoleAsync(String ^ roleName)
{
	return create_async([this, roleName]()
	{
		return rmData->ListUsersByRoleAsync(roleName);
	});
}

Address ^ scwin::scLocalDataService::GetAddress(String ^ addressId)
{
	ostringstream osql;
	osql << "select * from " << ADDRESSES << " where _id = '" << STRCONV(addressId) << "';";
	Address^ address = nullptr;
	int ret = DBM.ExecSqlNative(osql.str(), [this, &address](void* p, int argc, char** argv, char** azColName)
	{
		address = BuildAddress(argv);
		return SQLITE_OK;
	}, nullptr);
	return address;
}

IAsyncOperation<Address^>^ scwin::scLocalDataService::GetAddressAsync(String ^ addressId)
{
	return create_async([this, addressId]()
	{
		return GetAddress(addressId);
	});
}

MessageIds scwin::scLocalDataService::UpdateAddress(Address ^ address)
{
	if (address == nullptr)
	{
		return LogRetException(MessageIds::InvalidArgumentError);
	}
	ostringstream ostr;
	switch (address->ObjectState)
	{
	case DataState::Deleted:
		ostr << "update " << ADDRESSES << " set state = " << (int)DataState::Deleted << " where _id = '" << STRCONV(address->Id) << "';";
		break;
	case DataState::Added:
		ostr << "insert into " << ADDRESSES << " (_id, state, line1, line2, city, province, postalcode, country ) "
			<< " values ('" << STRCONV(address->Id) << "', " << (int)DataState::Added << ", '" << STRSAFECONV(address->Line1) << "', '" << STRSAFECONV(address->Line2) << "', '" << STRSAFECONV(address->City) << "', '"
			<< STRSAFECONV(address->Province) << "', '" << STRSAFECONV(address->PostalCode) << "', '" << STRSAFECONV(address->Country) << "');";
		break;
	case DataState::Edited:
		ostr << "update " << ADDRESSES
			<< " set state = " << (int)DataState::Edited << ", line1 = '" << STRSAFECONV(address->Line1) << "', line2 = '" << STRSAFECONV(address->Line2) << "', city = '" << STRSAFECONV(address->City) << "', "
			<< "province = '" << STRSAFECONV(address->Province) << "', postalcode = '" << STRSAFECONV(address->PostalCode) << "', country = '" << STRSAFECONV(address->Country) << "' where _id = '" << STRCONV(address->Id) << "';";
		break;
	}

	if (!DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
		return MessageIds::DBUpdateFailed;
	
	address->ObjectState = DataState::Unmodified;


	return MessageIds::Success;
}

task<MessageIds> scwin::scLocalDataService::UpdateAddressAsync(Address ^ address)
{
	return create_task([this, address]()
	{
		return UpdateAddress(address);
	});
}

IAsyncOperationWithProgress<IObservableVector<ProductCategory^>^, int>^ scwin::scLocalDataService::GetCategoryChildren(ProductCategory^ parent)
{
	return create_async([this, parent](progress_reporter<int> reporter, cancellation_token token) -> IObservableVector<ProductCategory^>^
	{
		auto t = this->replicator->GetDbMgr().GetCategoryChildrenAsync(parent);
		auto vector = t.get();
		return vector;
	});
}

ProductCategory ^ scwin::scLocalDataService::GetCategory(CONDITIONS condition)
{
	if (!condition.size())
	{
		return nullptr;
	}
	string cond = DBM.ConditionsAsWhereClause(PRODUCTCATEGORIES, condition);
	ostringstream sql;
	sql << "select a._id, name, desc, b.pcid from " << PRODUCTCATEGORIES << " a inner join "
		<< PRODUCTCATEGORYCATEGORYREL << " b on a._id = b.cid  where a.state < " << (int)DataState::Deleted << " and " + cond;
	ProductCategory^ cat = nullptr;
	int ret = DBM.ExecSqlNative(sql.str(), [&cat](void* p, int argc, char** argv, char** azColName)
	{
		cat = ref new ProductCategory(STRCONV(argv[0]));
		cat->CanRaiseEvents = false;
		cat->Name = STRCONV(argv[1]);
		cat->Desc = STRCONV(argv[2]);
		cat->HasChildren = false;
		cat->ParentId = STRCONV(argv[3]);
		cat->CanRaiseEvents = true;
		return SQLITE_OK;
	}, nullptr);
	return cat;
}

IAsyncOperation<ProductCategory^>^ scwin::scLocalDataService::GetCategoryAsync(CONDITIONS condition)
{
	return create_async([this, condition]() -> ProductCategory^
	{
		return GetCategory(condition);
	});
}

ProductCategory^ scwin::scLocalDataService::GetCategory(String^ id)
{
	ostringstream sql;
	sql << "select a._id, name, desc, b.pcid from " << PRODUCTCATEGORIES << " a inner join "
		<< PRODUCTCATEGORYCATEGORYREL << " b on a._id = b.cid  where a.state < " << (int)DataState::Deleted << " AND b.cid = '" << STRCONV(id) << "';";
	ProductCategory^ cat = nullptr;
	int ret = DBM.ExecSqlNative(sql.str(), [&cat](void* p, int argc, char** argv, char** azColName)
	{
		cat = ref new ProductCategory(STRCONV(argv[0]));
		cat->CanRaiseEvents = false;
		cat->Name = STRCONV(argv[1]);
		cat->Desc = STRCONV(argv[2]);
		cat->HasChildren = false;
		cat->ParentId = STRCONV(argv[3]);
		cat->CanRaiseEvents = true;
		return SQLITE_OK;
	}, nullptr);
	return cat;
}

IAsyncOperation<IVectorView<ProductCategory^>^>^ scwin::scLocalDataService::GetCategoryRootToCurrentPath(String ^ catId)
{
	return create_async([this, catId]() ->IVectorView<ProductCategory^>^
	{
		if (!catId)
			return nullptr;
		auto cats = ref new Vector<ProductCategory^>();
		String^ currCatId = catId;
		auto cat = GetCategory(currCatId);
		cat->HasChildren = false;
		cats->InsertAt(0, cat);
		currCatId = cat->ParentId;
		while (true)
		{
			auto cat = GetCategory(currCatId);
			if (cat == nullptr)
			{
				break;
			}
			cat->HasChildren = true;
			cats->InsertAt(0, cat);
			currCatId = cat->ParentId;

		}
		return cats->GetView();
	});
}

//IAsyncOperationWithProgress<ProductFamilyList^, int>^ scwin::scLocalDataService::GetProductFamiliesAsync()
//{
//	return create_async([this](progress_reporter<int> reporter, cancellation_token tok) -> ProductFamilyList^
//	{
//		auto pfamilies = ref new Vector<ProductFamily^>();
//		ostringstream sql;
//		sql << "select _id, name, desc, shortdesc, notes from " << PRODUCTFAMILIES << " where state != " << (int)DataState::Deleted << " order by name;";
//		int ret = DBM.ExecSqlNative(sql.str(), [this, pfamilies](void* p, int argc, char** argv, char** azColName)
//		{
//			ProductFamily^ fam = ref new ProductFamily(STRCONV(argv[0]));
//			fam->Name = STRCONV(argv[1]);
//			fam->Description = STRCONV(argv[2]);
//			fam->ShortDescription = STRCONV(argv[3]);
//			fam->Notes = STRCONV(argv[4]);
//			pfamilies->Append(fam);
//			return SQLITE_OK;
//		});
//		return pfamilies;
//	});
//}
//
//IAsyncOperationWithProgress<ProductList^, int>^ scwin::scLocalDataService::GetProductsOfFamily(String ^ productfamilyid)
//{
//	return create_async([this, productfamilyid](progress_reporter<int> reporter, cancellation_token tok) -> ProductList^
//	{
//		CONDITIONS cond;
//		cond.insert(make_pair("familyid", STRCONV(productfamilyid)));
//		auto products = GetProductsAsync(cond)->GetResults();
//		return products;
//	});
//}

#define PRODUCTFIELDDEFS " _id, skun, name, subname, desc, shortdesc, notes, pfamilyid, priceplanid, barcode, isstockless, uom "

ProductList ^ scwin::scLocalDataService::GetProducts()
{
	return GetProducts("");
}

IAsyncOperationWithProgress<ProductList^, int>^ scwin::scLocalDataService::GetProductsAsync()
{
	return create_async([this](progress_reporter<int> reporter, cancellation_token toc)->ProductList^
	{
		return GetProducts();
	});
}

ProductList ^ scwin::scLocalDataService::GetProducts(const CONDITIONS condition)
{
	auto products = ref new Vector<Product^>();
	ostringstream osql;
	osql << "select " << PRODUCTFIELDDEFS << " from " << PRODUCTTABLE << " a ";
	osql << " where state < " << (int)DataState::Deleted;
	const CONDITIONS& cond = condition;
	if (condition.size())
	{
		bool hascrit = false;
		auto fhascrit = [](bool b) { return (b ? " and " : " "); };
		string oper = "";
		string productid = "", productname = "";
		string categoryId = "", catname = "", familyid = "", familyname = "";
		string attrid = "", attrname = "", skun = "", pricelow = "", pricehigh = "";
		auto fop = cond.find("operator");
		if (fop != cond.end())
			oper = fop->second;
		fop = cond.find("categoryid");
		if (fop != cond.end())
			categoryId = fop->second;
		fop = cond.find("categoryname");
		if (fop != cond.end())
			catname = fop->second;
		fop = cond.find("familyid");
		if (fop != cond.end())
			familyid = fop->second;
		fop = cond.find("productid");
		if (fop != cond.end())
			productid = fop->second;
		fop = cond.find("productname");
		if (fop != cond.end())
			productname = fop->second;
		fop = cond.find("familyid");
		if (fop != cond.end())
			familyid = fop->second;
		fop = cond.find("familyname");
		if (fop != cond.end())
			familyname = fop->second;
		fop = cond.find("attributeid");
		if (fop != cond.end())
			attrid = fop->second;
		fop = cond.find("attributename");
		if (fop != cond.end())
			attrname = fop->second;
		fop = cond.find("skun");
		if (fop != cond.end())
			skun = fop->second;
		fop = cond.find("pricelow");
		if (fop != cond.end())
			pricelow = fop->second;
		fop = cond.find("pricehigh");
		if (fop != cond.end())
			pricehigh = fop->second;
		if (!familyid.empty())
		{
			osql << " and a.pfamilyid = '" << familyid << "' ";
			hascrit = true;
		}
		if (!productid.empty())
		{
			osql << fhascrit(hascrit) << " and a.productid = '" << productid << "' ";
			hascrit = true;
		}
		if (!productname.empty())
		{
			if (oper.empty())
			{
				osql << fhascrit(hascrit) << " and a.name = '" << productname << "' ";
			}
			else
			{
				if (oper == "begins")
				{
					osql << fhascrit(hascrit) << " and substr(a.name, 0, " << productname.size() << ") = '" << productname << "' ";
				}
				else if (oper == "like")
				{
					osql << fhascrit(hascrit) << " and a.name like '%" << productname << "%') ";
				}
			}
			hascrit = true;
		}
		if (!categoryId.empty())
		{
			osql << " and a._id in(select x.pid from " << CATEGORYTOPRODUCT << " x inner join(select cid from " << PRODUCTCATEGORYCATEGORYREL;
			osql << " where pcid = '" << categoryId << "' union select '" << categoryId << "') y on x.cid = y.cid) ";
		}
		//TODO ALL CONDITIONS HERE
	}
	osql << " order by ";
	auto fo = cond.find("orderby");
	if (fo != cond.end())
		osql << fo->second;
	else
		osql << "name";
	osql << ";";
	int ret = DBM.ExecSqlNative(osql.str(), [this, products](void*, int argc, char** argv, char** azColName)
	{
		//_id, skun, name, subname, desc, shortdesc, notes, pfamilyid, priceplanid, barcode, isstockless, uom 
		Product^ p = ref new Product(STRCONV(argv[0]));
		p->SKUN = STRCONV(argv[1]);
		p->Name = STRCONV(argv[2]);
		p->SubName = STRCONV(argv[3]);
		p->Description = STRCONV(argv[4]);
		p->ShortDescription = STRCONV(argv[5]);
		p->Notes = STRCONV(argv[6]);
		//p->Family = STRCONV(argv[7]);
		p->PricePlanId = STRCONV(argv[8]);
		//p->SetBarCode = STRCONV(argv[9]);
		p->IsStockLess = STRCONV(argv[10]) == "1";
		p->UOM = STRCONV(argv[11]);
		products->Append(p);
		// trigger title image loading
		LoadTitleImageIntoProductInstanceAsync(p);
		return SQLITE_OK;
	});
	return products;
}

/// conditions included: 
///		operator = [begins, like, equals]
///		categoryid = ?, categoryname = ?, familyid = ?, familyname = ?, 
///		attributeid = ?, attributename = ?, skun = ?
///		pricelow = ?, pricehigh = ? (if both are given, range is applied, other greater or lesser is applied)
IAsyncOperationWithProgress<ProductList^, int>^ scwin::scLocalDataService::GetProductsAsync(const CONDITIONS condition)
{
	return create_async([this, condition](progress_reporter<int> reporter, cancellation_token toc)->ProductList^
	{
		return GetProducts(condition);
	});
}

ProductList ^ scwin::scLocalDataService::GetProducts(const string & sqlcondition)
{
	auto products = ref new Vector<Product^>();
	ostringstream osql;
	osql << "select " << PRODUCTFIELDDEFS << " from " << PRODUCTTABLE << " a ";
	osql << "where a.state < " << (int)DataState::Deleted;
	if (!sqlcondition.empty())
		osql << " and (" << sqlcondition << ")";
	osql << " order by name;";
	int ret = DBM.ExecSqlNative(osql.str(), [this, products](void*, int argc, char** argv, char** azColName)
	{
		auto p = BuildProduct(argv);
		products->Append(p);
		return SQLITE_OK;
	});
	return products;
}

IAsyncOperation<ProductList^>^ scwin::scLocalDataService::GetProductsAsync(const string& sqlcondition)
{
	return create_async([this, &sqlcondition]()
	{
		return GetProducts(sqlcondition);
	});
}

Product ^ scwin::scLocalDataService::GetProduct(String ^ productId)
{
	if (StringHelpers::IsNullOrEmpty(productId)) 
		throw Exception::CreateException(E_INVALIDARG, "'productid' parameter is required.");

	ostringstream osql;
	osql << "select " << PRODUCTFIELDDEFS << " from " << PRODUCTTABLE << " a ";
	osql << "where a.state < " << (int)DataState::Deleted;
	osql << " and _id = '" << STRCONV(productId) << "';";
	Product^ prod = nullptr;
	int ret = DBSELECTORP(osql.str(), &prod)
	{
		prod = BuildProduct(argv);
		return SQLITE_OK;
	});
	return prod;
}

IAsyncOperation<Product^>^ scwin::scLocalDataService::GetProductAsync(String ^ productId)
{
	return create_async([this, productId]()
	{
		return GetProduct(productId);
	});
}

task<MessageIds> scwin::scLocalDataService::SaveProductWithCategoriesAsync(Product ^ prod, IVector<ProductCategory^>^ categories)
{
	return create_task([this, prod, categories]() -> MessageIds
	{
		if (!prod)
		{
			return MessageIds::InvalidArgumentError;
		}
		// check for min data
		// validate details part
		if (
			StringHelpers::IsNullOrEmpty(prod->Name) ||
			StringHelpers::IsNullOrEmpty(prod->SKUN) ||
			StringHelpers::IsNullOrEmpty(prod->ShortDescription)
			)
		{
			return MessageIds::MissingValuesError;
		}
		// if no categories set, same issues
		if (!categories || !categories->Size)
		{
			return MessageIds::MissingValuesError;
		}
		/*
		CREATE TABLE products
		(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0,
		skun TEXT NOT NULL CONSTRAINT ProdSkunIsUnique UNIQUE ON CONFLICT FAIL,
		name TEXT NOT NULL,
		subname TEXT,
		desc TEXT,
		shortdesc TEXT,
		notes TEXT,
		pfamilyid CHAR(32),
		priceplanid CHAR(32),
		barcode TEXT,
		isstockless INTEGER,
		uom TEXT)
		*/

		ostringstream ostr;

		//// check if already exists
		//ostr << "update " << PRODUCTTABLE << " set ";
		//ostr << "state = " << (int)DataState::Edited << ", ";
		//ostr << "skun = '" << STRCONV(prod->SKUN) << "', ";
		//ostr << "name = '" << STRCONV(prod->Name) << "', ";
		//ostr << "subname = '" << STRCONV(prod->SubName) << "', ";
		//ostr << "desc = '" << STRCONV(prod->Description) << "', ";
		//ostr << "shortdesc = '" << STRCONV(prod->ShortDescription) << "', ";
		//ostr << "notes = '" << STRCONV(prod->Notes) << "', ";
		////ostr << "pfamilyid = '" << STRCONV(prod->Family->Id) << "', ";
		//ostr << "priceplanid = '" << STRCONV(prod->PricePlanId) << "', ";
		//ostr << "isstockless = " << (prod->IsStockLess ? 1 : 0) << ", ";
		//ostr << "uom = '" << STRCONV(prod->UOM) << "' ";
		//ostr << "where _id = '" << STRCONV(prod->Id) << "'; ";

		//if (!DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
		//{
		//	return MessageIds::DBQueryFatalError;
		//}

		ostr << "insert or replace into " << PRODUCTTABLE << " (";
		ostr << "_id, _rev, state, skun, name, subname, desc, shortdesc, notes, pfamilyid, priceplanid, barcode, isstockless, uom ";
		ostr << ") ";
		ostr << "values (";
		ostr << "'" << STRCONV(prod->Id) << "', ";
		ostr << "'', ";
		ostr << (int)prod->ObjectState << ", ";
		ostr << "'" << STRCONV(prod->SKUN) << "', ";
		ostr << "'" << STRCONV(prod->Name) << "', ";
		ostr << "'" << STRCONV(prod->SubName) << "', ";
		ostr << "'" << STRCONV(prod->ShortDescription) << "', ";
		ostr << "'" << STRCONV(prod->Description) << "', ";
		ostr << "'" << STRCONV(prod->Notes) << "', ";
		ostr << "'', ";
		ostr << "'', ";
		ostr << "'', ";
		ostr << (prod->IsStockLess ? 1 : 0) << ", ";
		ostr << "'" << STRCONV(prod->UOM) << "' ";
		ostr << "); ";

		//}
		/*
		CREATE TABLE categorytoproducts
		(_id CHAR(32) PRIMARY KEY,
		_rev TEXT,
		state INTEGER NOT NULL DEFAULT 0,
		cid CHAR(32) NOT NULL,
		pid CHAR(32) NOT NULL, UNIQUE (cid, pid) )
		*/

		ostr << "update " << CATEGORYTOPRODUCT << " set state = " << (int)DataState::Deleted << " where pid = '" << STRCONV(prod->Id) << "' and cid not in ( ";
		for (UINT i = 0; i < categories->Size; i++)
		{
			ostr << "'" << STRCONV(categories->GetAt(i)->Id) << "' ";
			if (i < categories->Size - 1)
			{
				ostr << ", ";
			}
		}
		ostr << "); ";

		// place categories
		for (auto cat : categories)
		{
			string criteria = "_id = '" + STRCONV(cat->Id) + "'";
			if (!DBM.HasData(CATEGORYTOPRODUCT, criteria))
			{
				ostr << "insert into " << CATEGORYTOPRODUCT << " (_id, _rev, state, cid, pid) ";
				ostr << "values (";
				ostr << "'" << STRCONV(SysInfoHelpers::NewCouchDbId()) << "', ";
				ostr << "'', ";
				ostr << (int)DataState::Added << ", ";
				ostr << "'" << STRCONV(cat->Id) << "', ";
				ostr << "'" << STRCONV(prod->Id) << "' ";
				ostr << ");";
			}
		}

		//ostr << "commit;";
		if (!DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
		{
			return MessageIds::DBQueryFatalError;
		}
		
		prod->ObjectState = DataState::Unmodified;

		for (auto cat : categories)
		{
			cat->ObjectState = DataState::Unmodified;
		}
		return MessageIds::Success;
	});
}


task<MessageIds> scwin::scLocalDataService::SaveExistingProductAsync(Product ^ prod)
{
	return create_task([this, prod]() -> MessageIds
	{
		if (!prod)
		{
			return MessageIds::InvalidArgumentError;
		}
		// check for min data
		// validate details part
		if (
			StringHelpers::IsNullOrEmpty(prod->Name) ||
			StringHelpers::IsNullOrEmpty(prod->SKUN) ||
			StringHelpers::IsNullOrEmpty(prod->ShortDescription)
			)
		{
			return MessageIds::MissingValuesError;
		}

		// proceed to save
		ostringstream ostr;
		ostr << "update " << PRODUCTTABLE << " set ";
		ostr << "state = " << (int)DataState::Edited << ", ";
		ostr << "skun = '" << STRCONV(prod->SKUN) << "', ";
		ostr << "name = '" << STRCONV(prod->Name) << "', ";
		ostr << "subname = '" << STRCONV(prod->SubName) << "', ";
		ostr << "desc = '" << STRCONV(prod->Description) << "', ";
		ostr << "shortdesc = '" << STRCONV(prod->ShortDescription) << "', ";
		ostr << "notes = '" << STRCONV(prod->Notes) << "', ";
		ostr << "pfamilyid = '" << STRCONV(prod->Family->Id) << "', ";
		ostr << "priceplanid = '" << STRCONV(prod->Id) << "' ";
		ostr << "isstockless = " << (prod->IsStockLess ? 1 : 0) << ", ";
		ostr << "uom = '" << STRCONV(prod->UOM) << "';";
		if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
		{
			prod->ObjectState = DataState::Unmodified;
			return MessageIds::Success;
		}
		else
			return MessageIds::DBQueryFatalError;
	});
}

IAsyncOperation<IVector<ProductCategory^>^>^ scwin::scLocalDataService::GetProductCategoriesAsync(String ^ productId)
{
	return create_async([this, productId]() -> IVector<ProductCategory^>^
	{
		Vector<ProductCategory^>^ cats = ref new Vector<ProductCategory^>();
		ostringstream osql;
		osql << "select cid from " << CATEGORYTOPRODUCT << " where state < " << (int)DataState::Deleted << " AND pid = '" << STRCONV(productId) << "';";
		auto ret = DBM.IsSqlOk(DBM.ExecSqlNative(osql.str(), [this, cats](void*, int argc, char** argv, char** azColName)
		{
			auto cat = GetCategory(STRCONV(argv[0]));
			if (cat)
				cats->Append(cat);
			return SQLITE_OK;
		}));
		return cats;
	});
}

task<MessageIds> scwin::scLocalDataService::UpdateProductCategoriesAsync(String ^ productId, IVector<ProductCategory^>^ categories)
{
	return task<MessageIds>();
}

task<MessageIds> scwin::scLocalDataService::DeleteProductCategoriesAsync(String ^ productId, IVector<ProductCategory^>^ categories)
{
	return task<MessageIds>();
}

IVectorView<ProductPricePlan^>^ scwin::scLocalDataService::GetProductPricingPlans(String ^ productId)
{
	ostringstream osql;
	osql << "SELECT _id FROM " << PRODUCTPRICEPLANS;
	if (!StringHelpers::IsNullOrEmpty(productId))
		osql << " B INNER JOIN " << PRODUCTTABLE << " A ON A._id = B.priceplanid where A.state < 3 and B.state < 3 and B._id = '" << STRCONV(productId) << "'";
	Vector<ProductPricePlan^>^ pricePlans = ref new Vector<ProductPricePlan^>();
	DBM.ExecSqlNative(osql.str(), [this, pricePlans](void*, int argc, char** argv, char** colname)
	{
		String^ planId = STRCONV(argv[0]);
		auto pplan = create_task(GetProductPricingPlanAsync(planId)).get();
		if (pplan != nullptr)
			pricePlans->Append(pplan);
		return SQLITE_OK;
	});
	return pricePlans->GetView();
}

IAsyncOperation<IVectorView<ProductPricePlan^>^>^ scwin::scLocalDataService::GetProductPricingPlansAsync(String^ productId)
{
	return create_async([this, productId]()
	{
		return GetProductPricingPlans(productId);
	});
}

ProductPricePlan ^ scwin::scLocalDataService::GetProductPricingPlan(String ^ pricePlanId)
{
	ostringstream osql;
	osql << "select _id, plancode, plandesc, dateinit, dateended FROM " << PRODUCTPRICEPLANS << " where state < " << (int)DataState::Deleted << " AND _id = '" << STRCONV(pricePlanId) << "';";
	ProductPricePlan^ plan = nullptr;
	auto ret = DBM.IsSqlOk(DBM.ExecSqlNative(osql.str(), [&plan](void*, int argc, char** argv, char** azColName)
	{
		String^ id = STRCONV(argv[0]);
		String^ code = STRCONV(argv[1]);
		String^ desc = STRCONV(argv[2]);
		DateTime dateinit;
		dateinit.UniversalTime = std::stoll(argv[3], nullptr, 10);
		DateTime dateended;
		if (argv[4])
		{
			dateended.UniversalTime = std::stoll(argv[4], nullptr, 10);
		}
		plan = ref new ProductPricePlan(id);
		plan->CanRaiseEvents = false;
		plan->Code = code;
		plan->Description = desc;
		plan->StartDate = dateinit;
		plan->EndDate = dateended;
		plan->CanRaiseEvents = true;
		return SQLITE_OK;
	}));
	return plan;
}

IAsyncOperation<ProductPricePlan^>^ scwin::scLocalDataService::GetProductPricingPlanAsync(String ^ pricePlanId)
{
	return create_async([this, pricePlanId]()->ProductPricePlan^
	{
		return GetProductPricingPlan(pricePlanId);
	});
}

ProductTierPricePlans^ scwin::scLocalDataService::GetProductTierPricePlans(String ^ pricePlanId)
{
	ostringstream osql;
	osql << "select _id, pppid, unitprice, beginqty, endqty FROM " << PRODUCTPRICINGTIERDETAILS << " where state < " << (int)DataState::Deleted << " AND pppid = '" << STRCONV(pricePlanId) << "' order by beginqty;";
	ProductTierPricePlans^ tiers = nullptr;
	auto ret = DBM.IsSqlOk(DBM.ExecSqlNative(osql.str(), [&tiers](void*, int argc, char** argv, char** azColName)
	{
		if (tiers == nullptr)
			tiers = ref new Vector<PricePlanTierDetails^>();

		PricePlanTierDetails^ plan;
		String^ id = STRCONV(argv[0]);
		String^ pppid = STRCONV(argv[1]);
		float price = std::stof(argv[2], nullptr);
		int begQty = std::stoi(argv[3], nullptr, 10);
		int endQty = std::stoi(argv[4], nullptr, 10);
		plan = ref new PricePlanTierDetails(id);
		plan->CanRaiseEvents = false;
		plan->PricePlanId = pppid;
		plan->Price = price;
		plan->BeginQuantity = begQty;
		plan->EndQuantity = endQty;
		plan->CanRaiseEvents = true;

		tiers->Append(plan);

		return SQLITE_OK;
	}));
	return tiers;
}

IAsyncOperation<ProductTierPricePlans^>^ scwin::scLocalDataService::GetProductTierPricePlansAsync(String ^ pricePlanId)
{
	return create_async([this, pricePlanId]()->ProductTierPricePlans^
	{
		return GetProductTierPricePlans(pricePlanId);
	});
}

PricePlanStandardDetails ^ scwin::scLocalDataService::GetProductStandardPricePlan(String ^ pricePlanId)
{
	ostringstream osql;
	osql << "select _id, pppid, unitprice FROM " << PRODUCTPRICESTDDETAILS << " where state < " << (int)DataState::Deleted << " AND pppid = '" << STRCONV(pricePlanId) << "';";
	PricePlanStandardDetails^ plan = nullptr;
	auto ret = DBM.IsSqlOk(DBM.ExecSqlNative(osql.str(), [&plan](void*, int argc, char** argv, char** azColName)
	{
		String^ id = STRCONV(argv[0]);
		String^ pppid = STRCONV(argv[1]);
		auto price = std::stod(argv[2], nullptr);
		plan = ref new PricePlanStandardDetails(id);
		plan->CanRaiseEvents = false;
		plan->PricePlanId = pppid;
		plan->Price = (float)price;
		plan->CanRaiseEvents = true;
		return SQLITE_OK;
	}));
	return plan;
}

IAsyncOperation<PricePlanStandardDetails^>^ scwin::scLocalDataService::GetProductStandardPricePlanAsync(String ^ pricePlanId)
{
	return create_async([this, pricePlanId]()->PricePlanStandardDetails^
	{
		return GetProductStandardPricePlan(pricePlanId);
	});
}

GenericPriceDetailsList ^ scwin::scLocalDataService::GetPricePlanDetails(String ^ pricePlanId)
{
	auto view = ref new Vector<IPricePlanDetails^>();
	// first check the stnd table, else tiered
	auto stdDets = GetProductStandardPricePlan(pricePlanId);
	if (stdDets != nullptr)
		view->Append(stdDets);
	else
	{
		auto tieredList = GetProductTierPricePlans(pricePlanId);
		if (tieredList != nullptr)
		{
			for (auto pd : tieredList)
				view->Append(pd);
		}
	}
	return view;
}

IAsyncOperation<GenericPriceDetailsList^>^ scwin::scLocalDataService::GetPricePlanDetailsAsync(String ^ pricePlanId)
{
	return create_async([this, pricePlanId]() -> GenericPriceDetailsList^
	{
		return GetPricePlanDetails(pricePlanId);
	});
}

MessageIds scwin::scLocalDataService::InsertPricePlan(ProductPricePlan^ priceplan)
{
	if (priceplan->ObjectState == DataState::Unmodified)
	{
		LogException("priceplan object is not modified");
		return MessageIds::Success;
	}

	ostringstream ostr;
	ostr << "INSERT INTO " << PRODUCTPRICEPLANS << " ( ";
	ostr << "_id, state, plancode, plandesc, dateinit ";
	ostr << " ) VALUES ( ";
	ostr << "'" << STRCONV(priceplan->Id) << "', " << (int)DataState::Added << ", '" << STRCONV(priceplan->Code) << "', '" << STRCONV(priceplan->Description) << "', ";
	ostr << priceplan->StartDate.UniversalTime;
	ostr << " ); ";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::UpdatePricePlan(ProductPricePlan^ priceplan)
{
	if (priceplan->ObjectState == DataState::Unmodified)
	{
		LogException("Price plan object is not modified");
		return MessageIds::Success;
	}

	ostringstream ostr;
	ostr << "UPDATE " << PRODUCTPRICEPLANS << " SET ";
	ostr << "state = " << (int)DataState::Edited << ", ";
	ostr << "plancode = '" << STRCONV(priceplan->Code) << "', ";
	ostr << "plandesc = '" << STRCONV(priceplan->Description) << "', ";
	ostr << "dateinit = " << priceplan->StartDate.UniversalTime << ", ";
	if (priceplan->EndDate.UniversalTime > 0)
		ostr << "dateended = " << priceplan->EndDate.UniversalTime;
	else
		ostr << "dateended = NULL ";
	ostr << "WHERE _id = '" << STRCONV(priceplan->Id) << "'; ";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		priceplan->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::InsertStdPriceDetails(PricePlanStandardDetails ^ dets)
{
	if (dets->ObjectState == DataState::Unmodified)
	{
		LogException("Standard price plan object is not modified");
		return MessageIds::Success;
	}

	ostringstream ostr;
	ostr << "INSERT INTO " << PRODUCTPRICESTDDETAILS << "(";
	ostr << "_id, state, pppid, unitprice ";
	ostr << " ) ";
	ostr << " VALUES ";
	ostr << "( '" << STRCONV(dets->Id) << "', " << (int)DataState::Added << ", '" << STRCONV(dets->PricePlanId) << "', " << dets->Price << " );";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		dets->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::UpdateStdPriceDetails(PricePlanStandardDetails ^ dets)
{
	if (dets->ObjectState == DataState::Unmodified)
	{
		LogException("Standard price plan object is not modified");
		return MessageIds::Success;
	}

	if (HASFAILED(MarkPriceDetailsasDeleted(dets->PricePlanId, "std")))
		return MessageIds::DBUpdateFailed;

	ostringstream ostr;
	ostr << "UPDATE " << PRODUCTPRICESTDDETAILS << " SET ";
	ostr << "state = " << (int)DataState::Edited << ", ";
	ostr << "pppid = '" << STRCONV(dets->PricePlanId) << "', unitprice = " << dets->Price;
	ostr << " WHERE _id = '" << STRCONV(dets->Id) << "';";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		dets->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::InsertTieredPriceDetails(PricePlanTierDetails ^ dets)
{
	if (dets->ObjectState == DataState::Unmodified)
	{
		LogException("Tiered price plan object is not modified");
		return MessageIds::Success;
	}

	if (HASFAILED(MarkPriceDetailsasDeleted(dets->PricePlanId, "tiered")))
		return MessageIds::DBUpdateFailed;

	ostringstream ostr;
	ostr << "INSERT INTO " << PRODUCTPRICINGTIERDETAILS << "(";
	ostr << "_id, state, pppid, unitprice, beginqty, endqty ";
	ostr << " ) ";
	ostr << " VALUES ";
	ostr << "( '" << STRCONV(dets->Id) << "', " << (int)DataState::Added << ", '" << STRCONV(dets->PricePlanId) << "', " << dets->Price << ", " << dets->BeginQuantity << ", " << dets->EndQuantity << " );";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		dets->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::UpdateTieredPriceDetails(PricePlanTierDetails ^ dets)
{
	if (dets->ObjectState == DataState::Unmodified)
	{
		LogException("Tiered price plan object is not modified");
		return MessageIds::Success;
	}


	ostringstream ostr;
	ostr << "UPDATE " << PRODUCTPRICINGTIERDETAILS << " SET ";
	ostr << "state = " << (int)DataState::Edited << ", ";
	ostr << "pppid = '" << STRCONV(dets->PricePlanId) << "', unitprice = " << dets->Price << ", ";
	ostr << " beginqty = " << dets->BeginQuantity << ", ";
	ostr << " endqty = " << dets->EndQuantity;
	ostr << " WHERE _id = '" << STRCONV(dets->Id) << "';";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		dets->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::MarkPriceDetailsasDeleted(String ^ id, String ^ planType)
{
	ostringstream ostr;
	ostr << "update ";
	if (planType == "std")
		ostr << PRODUCTPRICESTDDETAILS;
	else
		ostr << PRODUCTPRICINGTIERDETAILS;
	ostr << " set state = " << (int)DataState::Deleted;
	ostr << " where _id = '" << STRCONV(id) << "';";
	if (!DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{
		return MessageIds::DBUpdateFailed;
	}
	return MessageIds::Success;
}

GeneralStatus ^ scwin::scLocalDataService::BuildStatus(char** rowData)
{
	auto gs = ref new GeneralStatus(std::strtol(rowData[0], nullptr, 10), STRCONV(rowData[2]), STRCONV(rowData[1]));
	gs->Description = STRCONV(rowData[3]);
	return gs;
}

Product^ scwin::scLocalDataService::BuildProduct(char ** rowData)
{
	//_id, skun, name, subname, desc, shortdesc, notes, pfamilyid, priceplanid, barcode, isstockless, uom 
	Product^ p = ref new Product(STRCONV(rowData[0]));
	p->SKUN = STRCONV(rowData[1]);
	p->Name = STRCONV(rowData[2]);
	p->SubName = STRCONV(rowData[3]);
	p->Description = STRCONV(rowData[4]);
	p->ShortDescription = STRCONV(rowData[5]);
	p->Notes = STRCONV(rowData[6]);
	if (rowData[7])
		p->Family = GetProductFamily(STRCONV(rowData[7]));
	p->PricePlanId = STRCONV(rowData[8]);
	//p->SetBarCode = STRCONV(argv[9]);
	p->IsStockLess = STRCONV(rowData[10]) == "1";
	p->UOM = STRCONV(rowData[11]);
	LoadTitleImageIntoProductInstanceAsync(p);
	return p;
}

ProductFamily ^ scwin::scLocalDataService::BuildProductFamilyDto(char ** rowData)
{
	//          0     1   2     3        4        5        6    
	//"select _id, _rev, name, desc, shortdesc, notes, pdefattrid from " << PRODUCTFAMILIES
	ProductFamily^ fam = ref new ProductFamily(STRCONV(rowData[0]));
	fam->CanRaiseEvents = false;
	fam->Name = STRCONV(rowData[2]);
	fam->Description = STRCONV(rowData[3]);
	fam->ShortDescription = STRCONV(rowData[4]);
	fam->Notes = STRCONV(rowData[5]);
	fam->DefaultAttributeId = STRCONV(rowData[6]);
	fam->CanRaiseEvents = true;
	return fam;
}

CustomerOrg ^ scwin::scLocalDataService::BuildCustomerOrg(char ** rowData)
{
	//_id, _rev, state, ccode, name, url, facebook, linkedin, notes, isactive, logo
	// 0     1     2      3     4     5     6            7     8       9        10
	CustomerOrg^ org = ref new CustomerOrg(STRCONV(rowData[0]));
	org->CanRaiseEvents = false;
	org->Code = STRCONV(rowData[3]);
	org->Name = STRCONV(rowData[4]);
	org->Website = STRCONV(rowData[5]);
	org->Facebook = STRCONV(rowData[6]);
	org->LinkedIn = STRCONV(rowData[7]);
	org->Notes = STRCONV(rowData[8]);
	org->IsActive = std::strtol(rowData[9], nullptr, 10) > 0;
	org->CanRaiseEvents = true;
	return org;
}

COrgCostCenter ^ scwin::scLocalDataService::BuildCostCenter(char ** rowData)
{
	// 0     1     2     3      4      5       6                 7                   8
	//_id, _rev, state, corgid, name, code, paymentmethodcode, paymenttermcode, invoicedesccode
	//   9          10              11             12      13     14    15       16        17
	// creditterm, creditlimit, shippingagentcode, phone, email, fax, mailaddr, billddr, shipaddr
	//  18          19       20
	// searchname, notes, isactive
	COrgCostCenter^ cc = ref new COrgCostCenter(STRCONV(rowData[0]));
	cc->CanRaiseEvents = false;
	cc->ParentOrgId = STRCONV(rowData[3]);
	cc->Name = STRCONV(rowData[4]);
	cc->Code = STRCONV(rowData[5]);
	cc->PaymentMethodCode = STRCONV(rowData[6]);
	cc->PaymentTermCode = STRCONV(rowData[7]);
	cc->InvoiceDescCode = STRCONV(rowData[8]);
	cc->CreditTerm = std::strtol(rowData[9], nullptr, 10);
	cc->CreditLimit = std::strtod(rowData[10], nullptr);
	cc->ShippingAgentCode = STRCONV(rowData[11]);
	cc->Phone = STRCONV(rowData[12]);
	cc->Email = STRCONV(rowData[13]);
	cc->Fax = STRCONV(rowData[14]);
	cc->MailingAddress = STRCONV(rowData[15]);
	cc->BillingAddress = STRCONV(rowData[16]);
	cc->ShippingAddress = STRCONV(rowData[17]);
	cc->SearchName = STRCONV(rowData[18]);
	cc->Notes = STRCONV(rowData[19]);
	cc->IsActive = std::strtol(rowData[20], nullptr, 10) > 0;
	cc->IsDefault = std::strtol(rowData[21], nullptr, 10) > 0;
	cc->CanRaiseEvents = true;
	return cc;
}

CustomerContact ^ scwin::scLocalDataService::BuildCustomerContact(char ** rowData)
{
	//   0    1    2       3           4        5        6       7        8         9      10       11
	// _id, _rev, state, corgccid, firstname, lastname, phone1, phone2, cellphone, email, facebook, linkedin
	CustomerContact^ c = ref new CustomerContact(STRCONV(rowData[0]));
	c->CanRaiseEvents = false;
	c->CostCenterId = STRCONV(rowData[3]);
	c->FirstName = STRCONV(rowData[4]);
	c->LastName = STRCONV(rowData[5]);
	c->Phone1 = STRCONV(rowData[6]);
	c->Phone2 = STRCONV(rowData[7]);
	c->Cellphone = STRCONV(rowData[8]);
	c->Email = STRCONV(rowData[9]);
	c->Facebook = STRCONV(rowData[10]);
	c->LinkedIn = STRCONV(rowData[11]);
	c->CanRaiseEvents = true;
	return c;
}

Address ^ scwin::scLocalDataService::BuildAddress(char ** rowData)
{
	//  0      1    2      3    4       5      6        7          8
	// _id, _ref, state, line1, line2, city, province, country, postalcode
	auto address = ref new Address(STRCONV(rowData[0]));
	address->CanRaiseEvents = false;
	address->Line1 = STRCONV(rowData[3]);
	address->Line2 = STRCONV(rowData[4]);
	address->City = STRCONV(rowData[5]);
	address->Province = STRCONV(rowData[6]);
	address->Country = STRCONV(rowData[7]);
	address->PostalCode = STRCONV(rowData[8]);
	address->CanRaiseEvents = true;
	return address;
}

ProductAttribute ^ scwin::scLocalDataService::BuildProductAttribute(char ** argv)
{
	auto a = ref new ProductAttribute(STRCONV(argv[0]));
	a->CanRaiseEvents = false;
	a->ParentId = STRCONV(argv[1]);
	a->Name = STRCONV(argv[2]);
	a->Description = STRCONV(argv[3]);
	a->Type = STRCONV(argv[4]);
	a->CanRaiseEvents = true;
	return a;
}

SalesOrder ^ scwin::scLocalDataService::BuildOrder(char ** rowData)
{
	//  0     1        2         3       4         5         6      7            8          9      10        
	//_id, onumber, corgccid, batchno, odate, cccontactid, repid, ototalgross, disccode, discnum, disctype, 
	// 11         12     13           14          15       16                  17
	//taxpct, taxvalue, currencyid, ototalfinal, onotes, customerexpecteddate, owner 

	auto o = ref new SalesOrder(STRCONV(rowData[0]));
	o->CanRaiseEvents = false;
	o->OrderNumber = STRCONV(rowData[1]);
	o->CostCenterId = STRCONV(rowData[2]);
	o->BatchNumber = STRCONV(rowData[3]);
	o->DateAndTime = ref new Date((__int64)std::strtol(rowData[4], nullptr, 10));
	o->ContactId = STRCONV(rowData[5]);
	o->RepId = STRCONV(rowData[6]);
	o->GrossAmount = std::strtod(rowData[7], nullptr);
	o->DiscountCode = STRCONV(rowData[8]);
	o->Discount = std::strtod(rowData[9], nullptr);
	o->DiscountType = (DiscountTypes)(std::strtol(rowData[10], nullptr, 10));
	o->TaxPct = std::strtod(rowData[11], nullptr);
	o->CurrencyId = STRCONV(rowData[13]);
	o->FinalTotal = std::strtod(rowData[14], nullptr);
	o->Notes = STRCONV(rowData[15]);
	o->CustomerExpectedDate = ref new Date((__int64)std::strtol(rowData[16], nullptr, 10));
	o->CanRaiseEvents = true;
	return o;
}

SaleOrderDetails ^ scwin::scLocalDataService::BuildOrderDetails(char ** rowData)
{
	SaleOrderDetails^ odet = ref new SaleOrderDetails(STRCONV(rowData[0]));
	//  0     1    2    3         4         5         6      7          8
	// _id, soid, pid, pqty, unitprice, grossprice, pdisc, pdisctype, netprice, 
	//    9               10               11
	// isexpedited, customerexpecteddate, owner
	odet->CanRaiseEvents = false;
	odet->OrderId = STRCONV(rowData[1]);
	odet->ProductId = STRCONV(rowData[2]);
	odet->Quantity = strtol(rowData[3], nullptr, 10);
	odet->UnitPrice = strtod(rowData[4], nullptr);
	odet->Gross = strtod(rowData[5], nullptr);
	odet->Discount = strtod(rowData[6], nullptr);
	odet->DiscountType = (DiscountTypes)strtol(rowData[7], nullptr, 10);
	odet->Net = strtod(rowData[8], nullptr);
	odet->CanRaiseEvents = true;
	return odet;
}

SalesOrderStatusHistory ^ scwin::scLocalDataService::BuildOrderStatusHistory(char ** rowData)
{
	// 0     1     2          3         4       5
	//_id, soid, ostatus, ostatusdate, notes, createdby
	SalesOrderStatusHistory^ hist = ref new SalesOrderStatusHistory(STRCONV(rowData[0]));
	hist->CanRaiseEvents = false;
	hist->OrderId = STRCONV(rowData[1]);
	hist->OrderStatus = GetGeneralStatus(ORDERSTATUS, std::strtol(rowData[2], nullptr, 10));
	hist->OrderStatusDate = ref new Date(strtol(rowData[3], nullptr, 10));
	hist->Notes = STRCONV(rowData[4]);
	hist->CreatedBy = STRCONV(rowData[5]);
	hist->CanRaiseEvents = true;
	return hist;
}

SalesOrderPaymentHistory ^ scwin::scLocalDataService::BuildOrderPaymentStatusHistory(char ** rowData)
{
	// 0     1         2                  3                   4            5       6               7
	//_id , soid , paymentstatus , paymentinstrument , paymentdate , currency , paidamount , paydetails 
	SalesOrderPaymentHistory^ hist = ref new SalesOrderPaymentHistory(STRCONV(rowData[0]));
	hist->CanRaiseEvents = false;
	hist->OrderId = STRCONV(rowData[0]);
	auto paystatusid = std::strtol(rowData[2], nullptr, 10);
	hist->PaymentStatus = GetGeneralStatus(PAYMENTSTATUS, paystatusid);
	hist->PaymentInstrument = GetGeneralStatus(PAYMENTINSTRUMENT, std::strtol(rowData[3], nullptr, 10));
	hist->PaymentDate = ref new Date(strtol(rowData[4], nullptr, 10));
	hist->CurrencyId = STRCONV(rowData[5]);
	hist->PayAmount = strtod(rowData[6], nullptr);
	hist->PayDetails = STRCONV(rowData[7]);
	hist->CanRaiseEvents = true;
	return hist;
}

ProductInventoryLocation ^ scwin::scLocalDataService::BuildInventoryLocation(char ** rowData)
{
	//                    0       1     2           3     4        5		6
	// inventorylocation( _id , _rev , state , location , code , contactid, addressid )
	ProductInventoryLocation^ obj = ref new ProductInventoryLocation(STRCONV(rowData[0]));
	obj->CanRaiseEvents = false;
	obj->Name = STRCONV(rowData[3]);
	obj->Code = STRCONV(rowData[4]);
	obj->ContactId= STRCONV(rowData[5]);
	obj->Address = GetAddress(STRCONV(rowData[6]));
	obj->CanRaiseEvents = true;
	return obj;
}

MessageIds scwin::scLocalDataService::DeleteUnusedAttributeAndValues()
{
	// TODO IMPORTANT : Ensure that this method won't disrupt the integrity of the 
	// couchdb data due to inadvertent delayed updates from individual remote apps.
	// first search values
	ostringstream osql;
	osql << "update productattributevalue set state = 3 where _id in (select _id from productattributevalue where pid not in (select _id from product union select _id from productfamily));";
	osql << "update productattribute set state = 3 where _id in(select aid from productattributevalue where pid not in(select _id from product union select _id from productfamily));";

	if (DBM.IsSqlOk(DBM.ExecSqlNative(osql.str())))
		return MessageIds::Success;

	return MessageIds::DBUpdateFailed;

}

task<MessageIds> scwin::scLocalDataService::SavePricePlan(ProductPricePlan ^ pricePlan, PricePlanStandardDetails ^ stdDetails)
{
	return create_task([this, pricePlan, stdDetails]() -> MessageIds
	{
		MessageIds ret = MessageIds::Success;
		if (!DBM.IsSqlOk(DBM.BeginTrans()))
			return MessageIds::DBTransactionBeginFailed;

		if (DBM.HasData(PRODUCTPRICEPLANS, "_id='" + STRCONV(pricePlan->Id) + "'"))
		{
			ret = UpdatePricePlan(pricePlan);
		}
		else
		{
			ret = InsertPricePlan(pricePlan);
		}
		if (DBM.HasData(PRODUCTPRICESTDDETAILS, "_id='" + STRCONV(stdDetails->Id) + "'"))
		{
			ret = UpdateStdPriceDetails(stdDetails);
			if (HASFAILED(ret))
			{
				DBM.RollbackTrans();
				return MessageIds::DBUpdateFailed;
			}
		}
		else
		{
			ret = InsertStdPriceDetails(stdDetails);
			if (HASFAILED(ret))
			{
				DBM.RollbackTrans();
				return MessageIds::DBUpdateFailed;
			}
		}

		if (!DBM.IsSqlOk(DBM.CommitTrans()))
			return MessageIds::DBTransactionCommitFailed;

		return MessageIds::Success;
	});
}

task<MessageIds> scwin::scLocalDataService::SavePricePlan(ProductPricePlan ^ pricePlan, ProductTierPricePlans ^ tierDetails)
{
	return create_task([this, pricePlan, tierDetails]() -> MessageIds
	{
		MessageIds ret = MessageIds::Success;
		if (!DBM.IsSqlOk(DBM.BeginTrans()))
			return MessageIds::DBTransactionBeginFailed;

		if (DBM.HasData(PRODUCTPRICEPLANS, "_id='" + STRCONV(pricePlan->Id) + "'"))
		{
			if (pricePlan->ObjectState != DataState::Unmodified)
			{
				ret = UpdatePricePlan(pricePlan);
			}
		}
		else
		{
			ret = InsertPricePlan(pricePlan);
		}

		for (PricePlanTierDetails^ det : tierDetails)
		{
			if (det->ObjectState == DataState::Deleted)
			{
				ret = MarkPriceDetailsasDeleted(det->Id);
				if (HASFAILED(ret))
				{
					DBM.RollbackTrans();
					return MessageIds::DBUpdateFailed;
				}
				continue;
			}
			if (DBM.HasData(PRODUCTPRICINGTIERDETAILS, "_id='" + STRCONV(det->Id) + "'"))
			{
				ret = UpdateTieredPriceDetails(det);
				if (HASFAILED(ret))
				{
					DBM.RollbackTrans();
					return MessageIds::DBUpdateFailed;
				}
			}
			else
			{
				ret = InsertTieredPriceDetails(det);
				if (HASFAILED(ret))
				{
					DBM.RollbackTrans();
					return MessageIds::DBUpdateFailed;
				}
			}
		}

		if (!DBM.IsSqlOk(DBM.CommitTrans()))
			return MessageIds::DBTransactionCommitFailed;

		return MessageIds::Success;
	});
}

task<MessageIds> scwin::scLocalDataService::AssociatePricePlanWithProduct(String ^ productId, String ^ pricePlanId)
{
	return create_task([this, productId, pricePlanId]()
	{
		ostringstream ostr;
		ostr << "update " << PRODUCTTABLE << " set state = " << (int)DataState::Edited << ", priceplanid = '" << STRCONV(pricePlanId) << "' where _id = '" << STRCONV(productId) << "';";
		if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
			return MessageIds::Success;
		else
			return MessageIds::DBUpdateFailed;
	});
}

//IAsyncOperation<IVector<ProductFamily^>^>^ scwin::scLocalDataService::GetProductFamiliesAsync(String ^ productId)
//{
//	return create_async([this, productId]() -> IVector<ProductFamily^>^
//	{
//		ostringstream ostr;
//		ostr << "select _id, _rev, name, desc, shortdesc, notes from " << PRODUCTFAMILIES;
//		if (!StringHelpers::IsNullOrEmpty(productId))
//		{
//			ostr << " a join " << PRODUCTTABLE << " b on a._id = b.pfamilyid ";
//		}
//		ostr << " where state < " << int(DataState::Deleted);
//		if (!StringHelpers::IsNullOrEmpty(productId))
//		{
//			ostr << " and b._id = '" << STRCONV(productId) << "' ";;
//		}
//		ostr << ";";
//		Vector<ProductFamily^>^ families = ref new Vector<ProductFamily^>();
//		auto ret = DBM.ExecSqlNative(ostr.str(), [this, families](void* p, int argc, char** argv, char** azColName)
//		{
//			auto fam = BuildProductFamilyDto(argv);
//			families->Append(fam);
//			return SQLITE_OK;
//		});
//		return families;
//	});
//}

IVector<ProductFamily^>^ scwin::scLocalDataService::GetProductFamilies(const string & criteria)
{
	Vector<ProductFamily^>^ families = ref new Vector<ProductFamily^>();
	ostringstream ostr;
	ostr << "select _id, _rev, name, desc, shortdesc, notes, pdefattrid from " << PRODUCTFAMILIES;
	ostr << " where state < " << int(DataState::Deleted);
	if (!criteria.empty())
	{
		ostr << " and ( " << criteria << " ) ";
	}
	ostr << ";";
	auto ret = DBM.ExecSqlNative(ostr.str(), [this, families](void* p, int argc, char** argv, char** azColName)
	{
		auto fam = BuildProductFamilyDto(argv);
		families->Append(fam);
		return SQLITE_OK;
	});
	return families;
}

IAsyncOperation<IVector<ProductFamily^>^>^ scwin::scLocalDataService::GetProductFamiliesAsync(const string& criteria)
{
	return create_async([this, criteria]() -> IVector<ProductFamily^>^
	{
		return GetProductFamilies(criteria);
	});
}

ProductFamily ^ scwin::scLocalDataService::GetProductFamily(String ^ familyId)
{
	ostringstream ostr;
	ostr << "select _id, _rev, name, desc, shortdesc, notes, pdefattrid from " << PRODUCTFAMILIES;
	ostr << " where state < " << int(DataState::Deleted) << " and _id = '" << STRCONV(familyId) << "';";
	ProductFamily^ fam = nullptr;
	auto ret = DBM.ExecSqlNative(ostr.str(), [this, &fam](void* p, int argc, char** argv, char** azColName)
	{
		fam = BuildProductFamilyDto(argv);
		return SQLITE_OK;
	});
	return fam;
}

MessageIds scwin::scLocalDataService::UpdateProductFamilyAndAssociateWithProducts(ProductFamily ^ productFamily, IVector<String^>^ associatedProducts)
{
	if (productFamily == nullptr)
		return LogRetException(MessageIds::InvalidArgumentError);
	// update product first
	ostringstream osql;
	if (DBM.HasData(PRODUCTFAMILIES, "_id = '" + STRCONV(productFamily->Id) + "'"))
	{
		osql << "update " << PRODUCTFAMILIES << " set ";
		osql << "name = '" << STRCONV(productFamily->Name) << "', ";
		osql << "desc = '" << STRCONV(productFamily->Description) << "', ";
		osql << "shortdesc = '" << STRCONV(productFamily->ShortDescription) << "', ";
		osql << "notes = '" << STRCONV(productFamily->Notes) << "', ";
		osql << "pdefattrid = '" << STRCONV(productFamily->DefaultAttributeId) << "', ";
		osql << "state = " << (int)DataState::Edited;
		osql << " where _id = '" << STRCONV(productFamily->Id) << "';";
	}
	else
	{
		osql << "insert into " << PRODUCTFAMILIES << " (";
		osql << "_id, state, name, desc, shortdesc, notes, pdefattrid ";
		osql << ")";
		osql << " VALUES (";
		osql << "'" << STRCONV(productFamily->Id) << "', ";
		osql << (int)DataState::Added << ", ";
		osql << "'" << STRCONV(productFamily->Name) << "', ";
		osql << "'" << STRCONV(productFamily->Description) << "', ";
		osql << "'" << STRCONV(productFamily->ShortDescription) << "', ";
		osql << "'" << STRCONV(productFamily->Notes) << "', ";
		osql << "'" << STRCONV(productFamily->DefaultAttributeId) << "' ";
		osql << ");";
	}

	if (associatedProducts != nullptr && associatedProducts->Size > 0)
	{
		for (auto prodId : associatedProducts)
		{
			osql << "update " << PRODUCTTABLE << " set pfamilyid = '" << STRCONV(productFamily->Id)
				<< "', state = " << (int)DataState::Edited << " where _id = '" << STRCONV(prodId) << "';";
		}
	}

	DBM.BeginTrans();
	if (DBM.IsSqlOk(DBM.ExecSqlNative(osql.str())))
	{
		DBM.CommitTrans();
		productFamily->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	else
	{
		DBM.RollbackTrans();
		return MessageIds::DBUpdateFailed;
	}
}

task<MessageIds> scwin::scLocalDataService::UpdateProductFamilyAndAssociateWithProductsAsync(ProductFamily ^ productFamily, IVector<String^>^ associatedProducts)
{
	return create_task([this, productFamily, associatedProducts]()
	{
		return UpdateProductFamilyAndAssociateWithProducts(productFamily, associatedProducts);
	});
}

MessageIds scwin::scLocalDataService::DisassociateProductFromFamily(String ^ productId)
{
	if (StringHelpers::IsNullOrEmpty(productId))
	{
		return LogRetException(MessageIds::InvalidArgumentError, "productid not given.");
	}
	ostringstream osql;
	osql << "update " << PRODUCTTABLE << " set pdefattrid = '', state = " << (int)DataState::Edited << " where _id = '" << STRCONV(productId) << "';";
	if (DBM.IsSqlOk(DBM.ExecSqlNative(osql.str())))
	{
		return MessageIds::Success;
	}
	return MessageIds::DBUpdateFailed;
}

MessageIds scwin::scLocalDataService::UpdateProductFamily(ProductFamily ^ family, String ^ productId)
{
	if (family == nullptr)
	{
		LogException("Invalid family arg");
		return MessageIds::InvalidArgumentError;
	}

	if (!family->IsValid)
	{
		LogException("Family arg instance is not successfully validated.");
		return MessageIds::MissingValuesError;
	}

	ostringstream osql;
	DBM.BeginTrans();
	if (DBM.HasData(PRODUCTFAMILIES, "_id = '" + STRCONV(family->Id) + "'"))
	{
		osql << "update " << PRODUCTFAMILIES << " set ";
		osql << "name = '" << STRSAFECONV(family->Name) << "', ";
		osql << "desc = '" << STRSAFECONV(family->Description) << "', ";
		osql << "shortdesc = '" << STRSAFECONV(family->ShortDescription) << "', ";
		osql << "notes = '" << STRSAFECONV(family->Notes) << "', ";
		osql << "state = " << (int)DataState::Edited;
		osql << " where _id = '" << STRCONV(family->Id) << "';";
	}
	else
	{
		osql << "insert into " << PRODUCTFAMILIES << " (";
		osql << "_id, state, name, desc, shortdesc, notes ";
		osql << ")";
		osql << " VALUES (";
		osql << "'" << STRCONV(family->Id) << "', ";
		osql << (int)DataState::Added << ", ";
		osql << "'" << STRSAFECONV(family->Name) << "', ";
		osql << "'" << STRSAFECONV(family->Description) << "', ";
		osql << "'" << STRSAFECONV(family->ShortDescription) << "', ";
		osql << "'" << STRSAFECONV(family->Notes) << "' ";
		osql << ");";
	}

	if (!StringHelpers::IsNullOrEmpty(productId))
	{

		osql << "update " << PRODUCTTABLE << " set pfamilyid = '" << STRCONV(family->Id)
			<< "', state = " << (int)DataState::Edited << " where _id = '" << STRCONV(productId) << "';";

	}
	auto ret = DBM.ExecSql(osql.str());
	if (DBM.IsSqlOk(ret))
	{
		family->ObjectState = DataState::Unmodified;
		DBM.CommitTrans();
		return MessageIds::Success;
	}
	else
	{
		DBM.RollbackTrans();
		return MessageIds::DBUpdateFailed;
	}
}

task<MessageIds> scwin::scLocalDataService::UpdateProductFamilyAsync(ProductFamily ^ family, String ^ productId)
{
	return create_task([this, productId, family]() -> MessageIds
	{
		return UpdateProductFamily(family, productId);
	});
}

MessageIds scwin::scLocalDataService::LinkProductToFamily(String ^ familyId, String ^ productId)
{
	if (StringHelpers::IsNullOrEmpty(familyId) || StringHelpers::IsNullOrEmpty(productId))
	{
		return LogRetException(MessageIds::InvalidArgumentError, "familyId or productId is not provided.");
	}
	ostringstream osql;
	osql << "update " << PRODUCTTABLE << " set pfamilyid='" << STRCONV(familyId) << "' where _id = '" << STRCONV(productId) << "';";
	if (!DBM.IsSqlOk(DBM.ExecSqlNative(osql.str())))
		return MessageIds::DBUpdateFailed;
	return MessageIds::Success;
}

MessageIds scwin::scLocalDataService::DeleteProductFamily(String ^ productFamilyId)
{
	if (StringHelpers::IsNullOrEmpty(productFamilyId))
	{
		return LogRetException(MessageIds::InvalidArgumentError);
	}
	ostringstream ostr;
	// first market to delete
	ostr << "update " << PRODUCTFAMILIES << " set state = " << (int)DataState::Deleted << " where _id = '" << STRCONV(productFamilyId) << "';";
	ostr << "update " << PRODUCTTABLE << " set state = " << (int)DataState::Edited << ", pfamilyid='' where pfamilyid = '" << STRCONV(productFamilyId) << "';";
	DBM.BeginTrans();
	if (DBM.IsSqlOk(DBM.ExecSqlNative(ostr.str())))
	{

		auto ret = DeleteUnusedAttributeAndValues();

		if (HASFAILED(ret))
		{
			DBM.RollbackTrans();
			return MessageIds::DBUpdateFailed;

		}

		DBM.CommitTrans();
		return MessageIds::Success;
	}
	else
	{
		DBM.RollbackTrans();
		return MessageIds::DBUpdateFailed;
	}
}

task<MessageIds> scwin::scLocalDataService::DeleteProductFamilyAsync(String ^ productFamilyid)
{
	return create_task([this, productFamilyid]()
	{
		return DeleteProductFamily(productFamilyid);
	});
}

IVector<Product^>^ scwin::scLocalDataService::GetProductsInFamily(String ^ familyId)
{
	auto vect = ref new Vector<Product^>();
	if (StringHelpers::IsNullOrEmpty(familyId))
		return vect;

	ostringstream osql;
	osql << "select _id, skun, name, subname, desc, shortdesc, notes, pfamilyid, priceplanid, barcode, isstockless, uom from " << PRODUCTTABLE << " a ";
	osql << "where a.state < " << (int)DataState::Deleted;
	osql << " and a.pfamilyid = '" << STRCONV(familyId) << "' ";
	osql << " order by a.name;";

	auto ret = DBM.ExecSqlNative(osql.str(), [this, vect](void* p, int argc, char** argv, char** azColName)
	{
		vect->Append(BuildProduct(argv));
		return SQLITE_OK;
	});

	return vect;

}

IAsyncOperation<IVector<Product^>^>^ scwin::scLocalDataService::GetProductsInFamilyAsync(String ^ familyId)
{
	return create_async([familyId, this]()
	{
		return GetProductsInFamily(familyId);
	});
}

IAsyncOperation<IVector<ProductAttribute^>^>^ scwin::scLocalDataService::GetAttributesAsync(String^ sqlwhere, String^ sqlorder)
{
	return create_async([this, sqlwhere, sqlorder]()->IVector<ProductAttribute^>^
	{
		ostringstream ostr;
		ostr << "select _id, paid, name, desc, attrtype from " << PRODUCTATTRIBUTE;
		if (!StringHelpers::IsNullOrEmpty(sqlwhere))
		{
			ostr << " where " << STRCONV(sqlwhere);
		}
		if (!StringHelpers::IsNullOrEmpty(sqlorder))
		{
			ostr << " order by " << STRCONV(sqlorder);
		}
		ostr << ";";

		Vector<ProductAttribute^>^ vect =
			ref new Vector<ProductAttribute^>();
		auto ret = DBM.ExecSqlNative(ostr.str(), [this, vect](void* p, int argc, char** argv, char** azColName)
		{
			auto a = BuildProductAttribute(argv);
			vect->Append(a);
			return SQLITE_OK;
		});
		return vect;
	});
}

ProductAttribute ^ scwin::scLocalDataService::GetProductAttribute(String ^ sqlwhere)
{
	if (StringHelpers::IsNullOrEmpty(sqlwhere))
	{
		throw Exception::CreateException((DWORD)MessageIds::InvalidArgumentError, "sqlwhere parameter is empty");
	}
	ostringstream osql;
	osql << "select _id, paid, name, desc, attrtype from " << PRODUCTATTRIBUTE;
	osql << " where " << STRCONV(sqlwhere);
	ProductAttribute^ a = nullptr;
	auto ret = DBM.ExecSqlNative(osql.str(), [this, &a](void*, int, char** argv, char**)
	{
		a = BuildProductAttribute(argv);
		return SQLITE_OK;
	});
	return a;
}

#include "DTODefaultLessComparer.h"

IAsyncOperation<IVector<ProductAttributeAndValue^>^>^ scwin::scLocalDataService::GetProductAttributesAsync(String^ productId)
{
	return create_async([this, productId]() -> IVector<ProductAttributeAndValue^>^
	{
		if (StringHelpers::IsNullOrEmpty(productId))
		{
			throw CreateException(MessageIds::InvalidArgumentError);
		}

		DBM.BeginTrans();

		ostringstream ostr;
		// get default attribute of the product
		ostr << "select pdefattrid from " << PRODUCTTABLE << " where _id = '" << STRCONV(productId) << "';";
		string defAttribId;
		DBM.ExecSqlNative(ostr.str(), [&defAttribId](void* p, int argc, char** argv, char** azColName)
		{
			defAttribId = argv[0];
			return SQLITE_OK;
		});

		ostr.str("");
		ostr << "SELECT a._id, a.paid, a.name, a.desc, a.attrtype, \
			b._id as avalueid, b.pid, b.avalue, b.avaluetype, b.uom \
			FROM " << PRODUCTATTRIBUTE << " a JOIN " << PRODUCTATTRIBUTEVALUE
			<< " b ON a._id = b.aid where a.state < 3 and b.state < 3 and b.pid = '" << STRCONV(productId) << "'";

		// build pair of classes and add to the map
		auto vect =
			ref new Vector<ProductAttributeAndValue^>();

		auto ret = DBM.ExecSqlNative(ostr.str(), [defAttribId, vect](void* p, int argc, char** argv, char** azColName)
		{
			auto a = ref new ProductAttribute(STRCONV(argv[0]));
			a->CanRaiseEvents = false;
			a->ParentId = STRCONV(argv[1]);
			a->Name = STRCONV(argv[2]);
			a->Description = STRCONV(argv[3]);
			a->Type = STRCONV(argv[4]);
			a->CanRaiseEvents = true;
			auto v = ref new GeneralAttributeValue(STRCONV(argv[5]));
			v->CanRaiseEvents = false;
			String^ valueType = STRCONV(argv[8]);
			if (valueType == "string")
				v->Value = STRCONV(argv[7]);
			else if (valueType == "number")
				v->Value = strtod(argv[7], nullptr);
			else if (valueType == "object")
				v->Value = STRCONV(argv[7]);
			else //TODO for dates as well.
				v->Value = STRCONV(argv[7]);
			v->DataType = SysInfoHelpers::JsonTypeToWinRtType(valueType);
			v->Uom = STRCONV(argv[9]);
			v->CanRaiseEvents = true;
			auto pav = ref new ProductAttributeAndValue(vect->Size + 1, a, v);
			pav->CanRaiseEvents = false;
			if (!strcmp(argv[0], defAttribId.c_str()))
				pav->IsProductDefaultAttribute = true;
			vect->Append(pav);
			pav->CanRaiseEvents = true;
			return SQLITE_OK;
		});

		DBM.CommitTrans();

		return vect;
	});
}

IAsyncOperation<ProductAttributeAndValue^>^ scwin::scLocalDataService::GetProductFamilyDefaultAttributeAsync(String ^ familyId)
{
	return create_async([this, familyId]() -> ProductAttributeAndValue^
	{
		return GetProductFamilyDefaultAttribute(familyId);
	});
}

ProductAttributeAndValue ^ scwin::scLocalDataService::GetProductFamilyDefaultAttribute(String ^ familyId)
{
	if (StringHelpers::IsNullOrEmpty(familyId))
	{
		throw CreateException(MessageIds::InvalidArgumentError);
	}

	DBM.BeginTrans();

	ostringstream ostr;
	// get default attribute of the product
	ostr << "select pdefattrid from " << PRODUCTFAMILIES << " where _id = '" << STRCONV(familyId) << "';";
	string defAttribId;
	DBM.ExecSqlNative(ostr.str(), [&defAttribId](void* p, int argc, char** argv, char** azColName)
	{
		defAttribId = argv[0];
		return SQLITE_OK;
	});

	ostr.str("");
	ostr << "SELECT a._id, a.paid, a.name, a.desc, a.attrtype, \
			b._id as avalueid, b.pid, b.avalue, b.avaluetype, b.uom \
			FROM " << PRODUCTATTRIBUTE << " a JOIN " << PRODUCTATTRIBUTEVALUE
		<< " b ON a._id = b.aid where a.state < 3 and b.state < 3 and b.pid = '" << STRCONV(familyId) << "'";

	// build pair of classes and add to the map
	ProductAttributeAndValue^ pav = nullptr;

	auto ret = DBM.ExecSqlNative(ostr.str(), [defAttribId, &pav](void* p, int argc, char** argv, char** azColName)
	{
		auto a = ref new ProductAttribute(STRCONV(argv[0]));
		a->CanRaiseEvents = false;
		a->ParentId = STRCONV(argv[1]);
		a->Name = STRCONV(argv[2]);
		a->Description = STRCONV(argv[3]);
		a->Type = STRCONV(argv[4]);
		a->CanRaiseEvents = true;
		auto v = ref new GeneralAttributeValue(STRCONV(argv[5]));
		v->CanRaiseEvents = false;
		String^ valueType = STRCONV(argv[8]);
		if (valueType == "string")
			v->Value = STRCONV(argv[7]);
		else if (valueType == "number")
			v->Value = strtod(argv[7], nullptr);
		else if (valueType == "object")
			v->Value = STRCONV(argv[7]);
		else //TODO for dates as well.
			v->Value = STRCONV(argv[7]);
		v->DataType = SysInfoHelpers::JsonTypeToWinRtType(valueType);
		v->Uom = STRCONV(argv[9]);
		v->CanRaiseEvents = true;
		pav = ref new ProductAttributeAndValue(1, a, v);
		if (!strcmp(argv[0], defAttribId.c_str()))
			pav->IsProductDefaultAttribute = true;
		return SQLITE_ABORT; // means if there are more than one record, only the first will be used, and loop will exit. We expect only a single record.
	});

	DBM.CommitTrans();

	return pav;
}

IAsyncOperation<IVector<String^>^>^ scwin::scLocalDataService::GetProductAttributeValuesDistinctAsync(String ^ attributeId, String^ attrvaluePattern)
{
	return create_async([this, attributeId, attrvaluePattern]() -> IVector<String^>^
	{
		ostringstream ostr;
		ostr << "select distinct avalue from productattributevalue where state < 3 "
			<< "and aid = '" << STRCONV(attributeId) << "' ";
		if (!StringHelpers::IsNullOrEmpty(attrvaluePattern))
		{
			ostr << " and avalue like '%" << STRSAFECONV(attrvaluePattern) << "%' ";
		}
		ostr << "order by avalue; ";
		Vector<String^>^ vect = ref new Vector<String^>();
		auto ret = DBM.ExecSqlNative(ostr.str(), [vect](void*, int, char** argv, char**)
		{
			vect->Append(STRCONV(argv[0]));
			return SQLITE_OK;
		});
		return vect;
	});
}

IAsyncOperation<IVector<String^>^>^ scwin::scLocalDataService::GetAttributeValueDistinctUomsAsync(String ^ attributeId, String^ uomPattern)
{
	return create_async([this, attributeId, uomPattern]() -> IVector<String^>^
	{
		ostringstream ostr;
		ostr << "select uom from productattributevalue where state < 3 "
			<< " and ifnull(uom,null) != null and aid = '" << STRCONV(attributeId) << "' ";
		if (!StringHelpers::IsNullOrEmpty(uomPattern))
		{
			ostr << " and uom like '%" << STRSAFECONV(uomPattern) << "%' ";
		}
		ostr << "order by avalue; ";
		Vector<String^>^ vect = ref new Vector<String^>();
		auto ret = DBM.ExecSqlNative(ostr.str(), [vect](void*, int, char** argv, char**)
		{
			vect->Append(STRCONV(argv[0]));
			return SQLITE_OK;
		});
		return vect;
	});
}

MessageIds scwin::scLocalDataService::UpdateProductAttribute(String ^ productId, ProductAttribute ^ obj, IProductAttributeValue ^ value)
{
	if (StringHelpers::IsNullOrEmpty(productId))
	{
		LogException("productid argument is not given.");
		return MessageIds::InvalidArgumentError;
	}
	if (obj == nullptr)
	{
		LogException("ProductAttribute argument is not provided.");
		return MessageIds::InvalidArgumentError;
	}

	if (!obj->IsValid || !value->IsValid)
	{
		LogException("Either the attribute or its value are not fully filled in.");
		return MessageIds::MissingValuesError;
	}

	String^ aid = obj->Id;
	bool bIsNewAttrib = false;
	ostringstream ostr;
	auto productAttr = GetProductAttribute("_id='" + aid + "' or name = '" + obj->Name + "'");
	bIsNewAttrib = productAttr == nullptr;

	if (bIsNewAttrib)
	{
		ostr << "insert into " << PRODUCTATTRIBUTE << " ( "
			<< "_id, state, paid, name, desc, attrtype "
			<< " ) values ( "
			<< "'" << STRCONV(obj->Id) << "', "
			<< (int)DataState::Added << ", '"
			<< STRCONV(obj->ParentId) << "', '"
			<< STRSAFECONV(obj->Name) << "', '"
			<< STRSAFECONV(obj->Description) << "', '"
			<< STRCONV(obj->Type) << "' "
			<< ");";
	}
	else
	{
		aid = productAttr->Id;
		ostr << "update " << PRODUCTATTRIBUTE << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "desc = '" << STRSAFECONV(obj->Description) << "' "
			<< "where _id='" << STRCONV(aid) << "'; ";

	}

	// If the attribute value is already set to the given product, then
	// we only need update the record if there is a change with UOM
	auto trimmedvalue = StringHelpers::Trim(value->Value->ToString());
	string avalue = STRSAFECONV(trimmedvalue);
	bool isThisAttribValueAlreadySet = false;
	isThisAttribValueAlreadySet = DBM.HasData(PRODUCTATTRIBUTEVALUE, "pid='" + STRCONV(productId) + "' and avalue = '" + avalue + "'");

	//ensure _id is unique.
	bool hasValue = false;
	if (bIsNewAttrib && (hasValue = DBM.HasData(PRODUCTATTRIBUTEVALUE, "_id='" + STRCONV(value->Id) + "'")))
	{
		throw CreateException(MessageIds::DuplicateId);
	}

	String^ uom;
	auto gvalue = dynamic_cast<GeneralAttributeValue^>(value);
	if (gvalue != nullptr)
		uom = gvalue->Uom;



	if (isThisAttribValueAlreadySet)
	{
		ostr << "update " << PRODUCTATTRIBUTEVALUE << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "aid = '" << STRCONV(aid) << "', "
			<< "uom = '" << STRSAFECONV(uom) << "' "
			<< "where pid = '" + STRCONV(productId) + "' and avalue = '" + avalue + "'; ";
	}
	else if (hasValue)
	{
		ostr << "update " << PRODUCTATTRIBUTEVALUE << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "pid = '" << STRCONV(productId) << "', "
			<< "aid = '" << STRCONV(aid) << "', "
			<< "avalue = '" << STRSAFECONV(value->Value->ToString()) << "', "
			<< "avaluetype = '" << STRCONV(SysInfoHelpers::WinRtTypeToJsonType(value->DataType)) << "', "
			<< "uom = '" << STRSAFECONV(uom) << "' "
			<< "where _id = '" << STRCONV(value->Id) << "';";
	}
	else
	{
		ostr << "insert into " << PRODUCTATTRIBUTEVALUE << " ( "
			<< "_id, state, pid, aid, avalue, avaluetype, uom "
			<< " ) "
			<< " values ("
			<< "'" << STRCONV(value->Id) << "', "
			<< (int)DataState::Added << ", '"
			<< STRCONV(productId) << "', '"
			<< STRCONV(aid) << "', '"
			<< STRSAFECONV(value->Value->ToString()) << "', '"
			<< STRCONV(SysInfoHelpers::WinRtTypeToJsonType(value->DataType)) << "', '"
			<< STRSAFECONV(uom) << "');";
	}

	auto ret = DBM.ExecSqlNative(ostr.str(), nullptr, nullptr, true);
	if (DBM.IsSqlOk(ret))
	{
		obj->ObjectState = DataState::Unmodified;
		value->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	else
	{
		return MessageIds::DBUpdateFailed;
	}

	return MessageIds::SuccessWithCaveat;
}

task<MessageIds> scwin::scLocalDataService::UpdateProductAttributeAsync(String^ productId, ProductAttribute ^ obj, IProductAttributeValue^ value)
{
	return create_task([this, productId, obj, value]() -> MessageIds
	{
		return UpdateProductAttribute(productId, obj, value);
	});
}

MessageIds scwin::scLocalDataService::UpdateProductFamilyDefaultAttribute(String ^ familyId, ProductAttribute ^ obj, IProductAttributeValue ^ value)
{
	if (StringHelpers::IsNullOrEmpty(familyId))
	{
		LogException("familyId argument is not given.");
		return MessageIds::InvalidArgumentError;
	}
	if (obj == nullptr)
	{
		LogException("ProductAttribute argument is not provided.");
		return MessageIds::InvalidArgumentError;
	}

	if (value == nullptr)
	{
		return LogRetException(MessageIds::MissingValuesError, "Attribute value is not provided.");
	}

	if (!obj->IsValid || !value->IsValid)
	{
		LogException("Either the attribute or its value are not fully filled in.");
		return MessageIds::MissingValuesError;
	}

	// what to do ?
	/*
	1. Insert or replace Product Attribute by matching id as well as name
	2. Insert attribute value. Use family id for "pid" field.
	3. Update pdefattrid field of product family.
	*/

	String^ aid = obj->Id;
	bool bIsNewAttrib = false;
	ostringstream ostr;
	auto productAttr = GetProductAttribute("_id='" + aid + "' or name = '" + obj->Name + "'");
	bIsNewAttrib = productAttr == nullptr;
	// check if attribute exist
	if (bIsNewAttrib)
	{
		ostr << "insert into " << PRODUCTATTRIBUTE << " ( "
			<< "_id, state, paid, name, desc, attrtype "
			<< " ) values ( "
			<< "'" << STRCONV(obj->Id) << "', "
			<< (int)DataState::Added << ", '"
			<< STRCONV(obj->ParentId) << "', '"
			<< STRSAFECONV(obj->Name) << "', '"
			<< STRSAFECONV(obj->Description) << "', '"
			<< STRCONV(obj->Type) << "' "
			<< ");";
	}
	else
	{
		aid = productAttr->Id;
		ostr << "update " << PRODUCTATTRIBUTE << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "desc = '" << STRSAFECONV(obj->Description) << "' "
			<< "where _id='" << STRCONV(aid) << "'; ";

	}
	// If the attribute value is already set to the given product, then
	// we only need update the record if there is a change with UOM
	auto trimmedvalue = StringHelpers::Trim(value->Value->ToString());
	string avalue = STRSAFECONV(trimmedvalue);
	bool isThisAttribValueAlreadySet = false;
	isThisAttribValueAlreadySet = DBM.HasData(PRODUCTATTRIBUTEVALUE, "pid='" + STRCONV(familyId) + "' and avalue = '" + avalue + "'");

	//ensure _id is unique.
	bool hasValue = false;
	if (bIsNewAttrib && (hasValue = DBM.HasData(PRODUCTATTRIBUTEVALUE, "_id='" + STRCONV(value->Id) + "'")))
	{
		throw CreateException(MessageIds::DuplicateId);
	}

	String^ uom;
	auto gvalue = dynamic_cast<GeneralAttributeValue^>(value);
	if (gvalue != nullptr)
		uom = gvalue->Uom;



	if (isThisAttribValueAlreadySet)
	{
		ostr << "update " << PRODUCTATTRIBUTEVALUE << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "aid = '" << STRCONV(aid) << "', "
			<< "uom = '" << STRSAFECONV(uom) << "' "
			<< "where pid = '" + STRCONV(familyId) + "' and avalue = '" + avalue + "'; ";
	}
	else if (hasValue)
	{
		ostr << "update " << PRODUCTATTRIBUTEVALUE << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "pid = '" << STRCONV(familyId) << "', "
			<< "aid = '" << STRCONV(aid) << "', "
			<< "avalue = '" << STRSAFECONV(value->Value->ToString()) << "', "
			<< "avaluetype = '" << STRCONV(SysInfoHelpers::WinRtTypeToJsonType(value->DataType)) << "', "
			<< "uom = '" << STRSAFECONV(uom) << "' "
			<< "where _id = '" << STRCONV(value->Id) << "';";
	}
	else
	{
		ostr << "insert into " << PRODUCTATTRIBUTEVALUE << " ( "
			<< "_id, state, pid, aid, avalue, avaluetype, uom "
			<< " ) "
			<< " values ("
			<< "'" << STRCONV(value->Id) << "', "
			<< (int)DataState::Added << ", '"
			<< STRCONV(familyId) << "', '"
			<< STRCONV(aid) << "', '"
			<< STRSAFECONV(value->Value->ToString()) << "', '"
			<< STRCONV(SysInfoHelpers::WinRtTypeToJsonType(value->DataType)) << "', '"
			<< STRSAFECONV(uom) << "');";
	}

	// update famly table
	ostr << "update " << PRODUCTFAMILIES << " set pdefattrid = '" << STRCONV(aid) << "' where _id = '" << STRCONV(familyId) << "';";

	auto ret = DBM.ExecSqlNative(ostr.str(), nullptr, nullptr, true);
	if (DBM.IsSqlOk(ret))
	{
		obj->ObjectState = DataState::Unmodified;
		value->ObjectState = DataState::Unmodified;
		return MessageIds::Success;
	}
	else
	{
		return MessageIds::DBUpdateFailed;
	}

	return MessageIds::SuccessWithCaveat;
}

task<MessageIds> scwin::scLocalDataService::UpdateProductFamilyDefaultAttributeAsync(String ^ familyId, ProductAttribute ^ obj, IProductAttributeValue ^ value)
{
	return create_task([this, familyId, obj, value]()
	{
		return UpdateProductFamilyDefaultAttribute(familyId, obj, value);
	});
}

MessageIds scwin::scLocalDataService::UpdateDefaultProductAttribute(String ^ productId, String ^ attribId)
{
	ostringstream ostr;
	ostr << "update " << PRODUCTTABLE << " set "
		<< "pdefattrid = '" << STRCONV(attribId) << "', "
		<< "state = " << (int)DataState::Edited
		<< " where _id = '" << STRCONV(productId) << "';";
	auto ret = DBM.ExecSqlNative(ostr.str());
	return DBM.IsSqlOk(ret) ? MessageIds::Success : MessageIds::DBUpdateFailed;
}

task<MessageIds> scwin::scLocalDataService::UpdateDefaultProductAttributeAsync(String ^ productId, String ^ attribId)
{
	return create_task([this, productId, attribId]() -> MessageIds
	{
		return UpdateDefaultProductAttribute(productId, attribId);
	});
}

task<MessageIds> scwin::scLocalDataService::DeleteProductAttributesAsync(String ^ attributeId)
{
	return create_task([this, attributeId]() -> MessageIds
	{
		// before doing this, we have to extract the avalues from PRODUCTATTRIBUTE table
		// and if this value already exist remove it.
		if (DBM.HasData(PRODUCTATTRIBUTEVALUE, "aid='" + STRCONV(attributeId) + "'"))
		{
			return MessageIds::IsUsed;
		}
		ostringstream ostr;
		ostr << "update " << PRODUCTATTRIBUTE << " set state = " << (int)DataState::Deleted
			<< " where _id = '" << STRCONV(attributeId) << "';";
		auto ret = DBM.ExecSqlNative(ostr.str());
		if (ISDBOK(ret))
			return MessageIds::Success;
		else
			return MessageIds::DBUpdateFailed;
	});
}

task<MessageIds> scwin::scLocalDataService::DeleteProductAttributeValueAsync(String^ attributeValueId)
{
	return create_task([this, attributeValueId]() -> MessageIds
	{
		ostringstream ostr;
		ostr << "update " << PRODUCTATTRIBUTEVALUE << " set state = " << (int)DataState::Deleted
			<< " where _id = '" << STRCONV(attributeValueId) << "';";
		auto ret = DBM.ExecSqlNative(ostr.str());
		if (ISDBOK(ret))
			return MessageIds::Success;
		else
			return MessageIds::DBUpdateFailed;
	});
}

IAsyncOperationWithProgress<IVector<ProductImage^>^, int>^ scwin::scLocalDataService::GetProductImagesAsync(String ^ productId)
{
	return create_async([this, productId](progress_reporter<int> reporter) -> IVector<ProductImage^>^
	{
		ostringstream osql;
		osql << "select _id, pid from " << PRODUCTMEDIA << " where state < " << (int)DataState::Deleted << " and pid = '" << STRCONV(productId) << "';";
		// get a count
		int imageCount = DBM.CountRows(PRODUCTMEDIA, (string)"pid = '" + STRCONV(productId) + "'");
		Vector<ProductImage^>^ images = ref new Vector<ProductImage^>();
		std::atomic_int imageIndex = 0;
		auto ret = DBM.ExecSqlNative(osql.str(), [this, reporter, imageCount, &imageIndex, &images](void* p, int argc, char** argv, char** azColName)
		{
			try
			{
				//imageIndex += 1;
				ProductImage^ img = create_task(GetProductImageAsync(STRCONV(argv[0]))).get();
				if (img != nullptr)
				{
					images->Append(img);
				}
			}
			catch (Exception^ ex)
			{
				LogEx(ex);
			}
			return SQLITE_OK;
		});
		reporter.report(100);
		return images;
	});
}

IAsyncOperation<ProductImage^>^ scwin::scLocalDataService::GetProductImageAsync(String ^ imageId)
{
	return create_async([this, imageId]() -> ProductImage^
	{
		ProductImage^ img = nullptr;
		ostringstream osql;
		osql << "select _id, pid, istitleimage, data, contenttype, title, desc from " << PRODUCTMEDIA << " where state < 3 and _id = '" << STRCONV(imageId) << "';";
		int retCode = this->GetCache().ExecSqlNative(osql.str(), [&img](void* p, int argc, char** argv, char** azColName)
		{
			// argc = 6;
			img = ref new ProductImage(STRCONV(argv[0]));
			img->CanRaiseEvents = false;
			img->Pid = STRCONV(argv[1]);
			img->IsTitleImage = STRCONV(argv[2]) == "1" ? true : false;
			img->Data = STRCONV(argv[3]);
			img->ContentType = STRCONV(argv[4]);
			img->Title = STRCONV(argv[5]);
			img->Description = STRCONV(argv[6]);
			img->CanRaiseEvents = true;
			return SQLITE_OK;
		});
		if (DBFAILED(retCode))
		{
			ThrowException(MessageIds::DBQueryFatalError);
		}
		img->BeginDataToImageConversion();

		return img;
	});
}

IAsyncOperation<bool>^ scwin::scLocalDataService::LoadTitleImageIntoProductInstanceAsync(Product ^ product)
{
	return create_async([this, product]() -> bool
	{
		product->IsTitleImageLoading = true;
		ostringstream osql;
		osql << "select _id from " << PRODUCTMEDIA << " where state < 3 and pid = '" << STRCONV(product->Id) << "' and istitleimage = 1;";
		string imageId;
		auto ret = DBM.ExecSqlNative(
			osql.str(),
			[&imageId](void* p, int argc, char** argv, char** azColName)
		{
			imageId = argv[0];
			return SQLITE_OK;
		}
		);
		if (!DBM.IsSqlOk(ret))
		{
			product->IsTitleImageLoading = false;
			return false;
		}
		if (imageId.empty())
		{
			product->IsTitleImageLoading = false;
			return false;
		}
		auto pi = create_task(GetProductImageAsync(StringHelpers::ConvertToString(imageId.c_str()))).get();
		product->TitleImage = pi;
		pi->BeginDataToImageConversion();
		product->IsTitleImageLoading = false;
		return true;
	});
}

task<MessageIds> scwin::scLocalDataService::SaveProductImageAsync(ProductImage ^ image)
{
	DataState state = (DataState)image->ObjectState;

	if (state == DataState::Deleted)
	{
		return DeleteProductImageAsync(image->Id);
	}

	return create_task([this, state, image]()
	{
		if (image == nullptr)
		{
			return LogRetException(MessageIds::InvalidArgumentError);
		}

		// ensure image is converted to data image->BeginConvertFromFileToData()


		string const id = STRCONV(image->Id);
		string const pid = STRCONV(image->Pid);
		int const istitleimage = image->IsTitleImage ? 1 : 0;
		string const data = STRCONV(image->Data);
		string const ctype = STRCONV(image->ContentType);
		string const title = STRCONV(image->Title);
		string const desc = STRCONV(image->Description);

		ostringstream osql;
		if (DBM.HasData(PRODUCTMEDIA, "_id='" + id + "'"))
		{
			osql << "UPDATE " << PRODUCTMEDIA << " SET ";
			osql << "state = " << (INT)DataState::Edited << ", ";
			osql << "pid = '" << pid << "', ";
			osql << "istitleimage = " << istitleimage << ", ";
			osql << "data = '" << data << "', ";
			osql << "contenttype = '" << ctype << "', ";
			osql << "title = '" << title << "', ";
			osql << "desc = '" << desc << "' ";
			osql << "where _id = '" << id << "'";
		}
		else
		{
			osql << "INSERT INTO " << PRODUCTMEDIA << " (_id, state, pid, istitleimage, data, contenttype, title, desc ) ";
			osql << "VALUES ('" << id << "'," << (INT)DataState::Added << ", '" << pid << "', ";
			osql << istitleimage << ", '" << data << "', '" << ctype << "', '" << title << "', '" << desc << "')";
		}
		auto ret = DBM.ExecSql(osql.str());
		if (DBFAILED(ret))
		{
			return LogRetException(MessageIds::DBUpdateFailed);
		}

		image->ObjectState = DataState::Unmodified;

		return MessageIds::Success;
	});
}

task<MessageIds> scwin::scLocalDataService::DeleteProductImageAsync(String ^ imageId)
{
	return create_task([this, imageId]()
	{
		if (StringHelpers::IsNullOrEmpty(imageId))
			return LogRetException(MessageIds::InvalidArgumentError);

		ostringstream osql;
		osql << "UPDATE " << PRODUCTMEDIA << " SET state = " << (int)DataState::Deleted
			<< " WHERE _id = '" << STRCONV(imageId) << "'";
		auto ret = DBM.ExecSql(osql.str());
		if (DBFAILED(ret))
		{
			return LogRetException(MessageIds::DBQueryFatalError);
		}
		return MessageIds::Success;
	});
}

IMap<String^, ProductInventory^>^ scwin::scLocalDataService::GetProductsInventories()
{
	IMap<String^, ProductInventory^>^ map = ref new Map<String^, ProductInventory^>();

	return map;
}

ProductInventory ^ scwin::scLocalDataService::GetProductInventory(String ^ productId)
{
	ProductInventory^ inv = nullptr;

	return inv;
}

MessageIds scwin::scLocalDataService::UpdateInventory(int movementType, String ^ productId, String ^ location, uint64 qty)
{
	/* 
	a. Make sure that qty is within the limits specified in InventoryProperties table. 
	b. by looking at the movementType variable, make a "in" or "out" entry in inventorytrx table
	c. Update the net qty for the location in activeinventory table.
	*/

	return MessageIds::NotImplemented;
}

IVector<CustomerOrg^>^ scwin::scLocalDataService::GetCustomerOrganizations(String ^ sqlcondition)
{
	auto v = ref new Vector<CustomerOrg^>();
	ostringstream osql;
	osql << "select * from " << CUSTOMERORGS;
	if (!StringHelpers::IsNullOrEmpty(sqlcondition))
		osql << " where " << STRCONV(sqlcondition) << " and state < 3 ;";
	else
		osql << " where state < 3; ";
	auto ret = DBM.ExecSqlNative(osql.str(), [this, v](void* p, int argc, char** argv, char** azColName)
	{
		v->Append(BuildCustomerOrg(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<CustomerOrg^>^>^ scwin::scLocalDataService::GetCustomerOrganizationsAsync(String ^ sqlcondition)
{
	return create_async([this, sqlcondition]() -> IVector<CustomerOrg^>^
	{
		return GetCustomerOrganizations(sqlcondition);
	});
}

CustomerOrg ^ scwin::scLocalDataService::GetCustomerOrganization(String ^ ccid)
{
	ostringstream osql;
	osql << "select * from " << CUSTOMERORGS << " where _id = '" << STRCONV(ccid) << "';";
	CustomerOrg^ c = nullptr;
	auto ret = DBM.ExecSqlNative(osql.str(), [this, &c](void* p, int argc, char** argv, char** azColName)
	{
		c = BuildCustomerOrg(argv);
		return SQLITE_OK;
	});
	return c;
}

IAsyncOperation<CustomerOrg^>^ scwin::scLocalDataService::GetCustomerOrganizationAsync(String ^ ccid)
{
	return create_async([this, ccid]() -> CustomerOrg^
	{
		return GetCustomerOrganization(ccid);
	});
}

MessageIds scwin::scLocalDataService::UpdateCustomerOrganization(CustomerOrg ^ org)
{
	if (org->ObjectState == DataState::Deleted)
		return DeleteCustomerOrganization(org->Id);
	// check if all required fields are provided
	if (!org->IsValid)
		return MessageIds::MissingValuesError;

	const string id = STRCONV(org->Id);
	const string ccode = STRSAFECONV(org->Code);
	const string name = STRSAFECONV(org->Name);
	const string url = STRSAFECONV(org->Website);
	const string facebook = STRSAFECONV(org->Facebook);
	const string linkedin = STRSAFECONV(org->LinkedIn);
	const string notes = STRSAFECONV(org->Notes);
	const int isactive = org->IsActive ? 1 : 0;
	const DataState state = org->ObjectState;


	ostringstream osql;
	if (DBM.HasData(CUSTOMERORGS, "_id = '" + id + "'"))
	{
		osql << "update " << CUSTOMERORGS << " SET "
			<< "state = " << (int)DataState::Edited << ", "
			<< "ccode = '" << ccode << "', "
			<< "name = '" << name << "', "
			<< "url = '" << url << "', "
			<< "facebook = '" << facebook << "', "
			<< "linkedin = '" << linkedin << "', "
			<< "notes = '" << notes << "', "
			<< "isactive = " << isactive << " "
			<< " where _id = '" << id << "';";
	}
	else
	{
		osql << "insert into " << CUSTOMERORGS << " ( "
			<< "_id, state, ccode, name, url, facebook, linkedin, notes, isactive ) "
			<< " VALUES ( "
			<< "'" << id << "', " << (int)DataState::Added << ", '" << ccode << "', '" << name << "', '" << url << "', '" << facebook << "', '" << linkedin << "', '" << notes << "', " << isactive
			<< ");";
	}
	auto ret = DBM.ExecSqlNative(osql.str());
	if (ret == 19)
		return MessageIds::UniqueConstraintFailed;
	if (!DBM.IsSqlOk(ret))
		return MessageIds::DBUpdateFailed;
	org->ObjectState = DataState::Unmodified;
	return MessageIds::Success;
}

task<MessageIds> scwin::scLocalDataService::UpdateCustomerOrganizationAsync(CustomerOrg ^ org)
{
	if (org->ObjectState == DataState::Deleted)
		return DeleteCustomerOrganizationAsync(org->Id);

	return create_task([this, org]() -> MessageIds
	{
		return UpdateCustomerOrganization(org);
	});
}

MessageIds scwin::scLocalDataService::DeleteCustomerOrganization(String ^ orgId)
{
	return MessageIds::NotImplemented;
}

task<MessageIds> scwin::scLocalDataService::DeleteCustomerOrganizationAsync(String ^ orgId)
{
	// TODO delete cannot happen if there are orders existing....
	return create_task([this, orgId]()
	{

		return MessageIds::NotImplemented;
	});
}

CustomerOrg ^ scwin::scLocalDataService::GetCustomerOrgOfCostCenter(String ^ costcenterId)
{
	ostringstream osql;
	osql << "select * from customersorg c join corgcostcenter cc on c._Id = cc.corgid "
		<< "where cc._Id = '" << STRCONV(costcenterId) << "';";
	CustomerOrg^ c = nullptr;
	auto ret = DBSELECTORP(osql.str(), &c)
	{
		c = BuildCustomerOrg(argv);
		return SQLITE_OK;
	});
	return c;
}

IVector<COrgCostCenter^>^ scwin::scLocalDataService::GetCustomerCostCenters(String ^ sqlcondition)
{
	auto v = ref new Vector<COrgCostCenter^>();
	ostringstream osql;
	osql << "select * from " << CORGCOSTCENTER;
	if (!StringHelpers::IsNullOrEmpty(sqlcondition))
		osql << " where " << STRCONV(sqlcondition) << ";";
	auto ret = DBM.ExecSqlNative(osql.str(), [this, v](void* p, int argc, char** argv, char** azColName)
	{
		v->Append(BuildCostCenter(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<COrgCostCenter^>^>^ scwin::scLocalDataService::GetCustomerCostCentersAsync(String ^ sqlcondition)
{
	return create_async([this, sqlcondition]() -> IVector<COrgCostCenter^>^
	{
		return GetCustomerCostCenters(sqlcondition);
	});
}

COrgCostCenter ^ scwin::scLocalDataService::GetCustomerCostCenter(String ^ ccid)
{
	COrgCostCenter^ c = nullptr;
	ostringstream osql;
	osql << "select * from " << CORGCOSTCENTER << " where _id = '" << STRCONV(ccid) << "';";
	auto ret = DBM.ExecSqlNative(osql.str(), [this, &c](void* p, int argc, char** argv, char** azColName)
	{
		c = BuildCostCenter(argv);
		return SQLITE_OK;
	});
	return c;
}

IAsyncOperation<COrgCostCenter^>^ scwin::scLocalDataService::GetCustomerCostCenterAsync(String ^ ccid)
{
	return create_async([this, ccid]() -> COrgCostCenter^
	{
		return GetCustomerCostCenter(ccid);
	});
}

MessageIds scwin::scLocalDataService::UpdateCustomerCostCenter(COrgCostCenter ^ costcenter)
{
	if (costcenter->ObjectState == DataState::Deleted)
	{
		return DeleteCustomerCostCenter(costcenter->Id);
	}

	const string id = STRCONV(costcenter->Id);
	const string corgid = STRCONV(costcenter->ParentOrgId);
	const string name = STRSAFECONV(costcenter->Name);
	const string code = STRSAFECONV(costcenter->Code);
	const string paymentmethodcode = STRSAFECONV(costcenter->PaymentMethodCode);
	const string paymenttermcode = STRCONV(costcenter->PaymentTermCode);
	const string invoicedesccode = STRSAFECONV(costcenter->InvoiceDescCode);
	const int creditterm = costcenter->CreditLimit;
	const auto creditlimit = costcenter->CreditLimit;
	const string shippingagentcode = STRSAFECONV(costcenter->ShippingAgentCode);
	const string phone = STRSAFECONV(costcenter->Phone);
	const string email = STRSAFECONV(costcenter->Email);
	const string fax = STRSAFECONV(costcenter->Fax);
	const string mailaddr = STRCONV(costcenter->MailingAddress);
	const string billaddr = STRCONV(costcenter->BillingAddress);
	const string shipaddr = STRCONV(costcenter->ShippingAddress);
	const string searchname = STRCONV(costcenter->SearchName);
	const string notes = STRCONV(costcenter->Notes);
	const bool isactive = costcenter->IsActive;
	const bool isdefault = costcenter->IsDefault;

	ostringstream osql;
	if (DBM.HasData(CORGCOSTCENTER, "_id = '" + id + "'"))
	{
		osql << "update " << CORGCOSTCENTER << " SET "
			<< "state = " << (int)DataState::Edited << ", "
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
			<< "isdefault = " << (isdefault ? 1 : 0) << " "
			<< " where _id = '" << id << "';";
	}
	else
	{
		osql << "insert into " << CORGCOSTCENTER << " ( "
			<< "_id, state, corgid, name, code, paymentmethodcode, paymenttermcode, invoicedesccode, creditterm, "
			<< "creditlimit, shippingagentcode, phone, email, fax, "
			<< "mailaddr, billaddr, shipaddr, searchname, notes, isactive, isdefault ) "
			<< " VALUES ( "
			<< "'" << id << "', " << (int)DataState::Added << ", '" << corgid << "', '" << name << "', '" << code << "', '" << paymentmethodcode
			<< "', '" << paymenttermcode << "', '" << invoicedesccode << "', " << creditterm << ", " << creditlimit
			<< ", '" << shippingagentcode << "', '" << phone << "', '" << email << "', '"
			<< fax << "', '" << mailaddr << "', '" << billaddr << "', '" << shipaddr << "', '"
			<< searchname << "', '" << notes << "', " << (isactive ? 1 : 0) << ", " << (isdefault ? 1 : 0)
			<< " );";
	}

	auto ret = DBM.ExecSqlNative(osql.str());
	if (!DBM.IsSqlOk(ret))
		return MessageIds::DBUpdateFailed;
	costcenter->ObjectState = DataState::Unmodified;
	return MessageIds::Success;

}

task<MessageIds> scwin::scLocalDataService::UpdateCustomerCostCenterAsync(COrgCostCenter ^ costcenter)
{
	if (costcenter->ObjectState == DataState::Deleted)
	{
		return DeleteCustomerCostCenterAsync(costcenter->Id);
	}

	return create_task([this, costcenter]() -> MessageIds
	{
		return UpdateCustomerCostCenter(costcenter);
	});
}

MessageIds scwin::scLocalDataService::DeleteCustomerCostCenter(String ^ orgId)
{
	return MessageIds::NotImplemented;
}

task<MessageIds> scwin::scLocalDataService::DeleteCustomerCostCenterAsync(String ^ ccid)
{
	// TODO delete cannot happen if there are orders existing....
	return create_task([this, ccid]()
	{
		return MessageIds::NotImplemented;
	});
}

IVector<CustomerContact^>^ scwin::scLocalDataService::GetCustomerContacts(String ^ sqlcondition)
{
	auto v = ref new Vector<CustomerContact^>();
	ostringstream osql;
	osql << "select * from " << CUSTOMERORGCONTACTS;
	if (!StringHelpers::IsNullOrEmpty(sqlcondition))
		osql << " where " << STRCONV(sqlcondition) << ";";
	auto ret = DBM.ExecSqlNative(osql.str(), [this, v](void* p, int argc, char** argv, char** azColName)
	{
		v->Append(BuildCustomerContact(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<CustomerContact^>^>^ scwin::scLocalDataService::GetCustomerContactsAsync(String ^ sqlcondition)
{
	return create_async([this, sqlcondition]() -> IVector<CustomerContact^>^
	{
		return GetCustomerContacts(sqlcondition);
	});
}

CustomerContact ^ scwin::scLocalDataService::GetCustomerContact(String ^ id)
{
	CustomerContact^ c = nullptr;
	ostringstream osql;
	osql << "select * from " << CUSTOMERORGCONTACTS << " where _id = '" << STRCONV(id) << "';";
	auto ret = DBM.ExecSqlNative(osql.str(), [this, &c](void* p, int argc, char** argv, char** azColName)
	{
		c = BuildCustomerContact(argv);
		return SQLITE_OK;
	});
	return c;
}

IAsyncOperation<CustomerContact^>^ scwin::scLocalDataService::GetCustomerContactAsync(String ^ id)
{
	return create_async([this, id]() -> CustomerContact^
	{
		return GetCustomerContact(id);
	});
}

MessageIds scwin::scLocalDataService::UpdateCustomerContact(String ^ costCenterId, CustomerContact ^ contact)
{
	if (StringHelpers::IsNullOrEmpty(costCenterId) || contact == nullptr)
		return MessageIds::InvalidArgumentError;

	if (StringHelpers::IsNullOrEmpty(contact->CostCenterId))
		contact->CostCenterId = costCenterId;

	if (!contact->IsValid)
		return MessageIds::MissingValuesError;

	if (contact->ObjectState == DataState::Deleted)
	{
		return DeleteCustomerContact(costCenterId, contact->Id);
	}

	const string id = STRCONV(contact->Id);
	const string corgccid = STRCONV(contact->CostCenterId);
	const string firstname = STRSAFECONV(contact->FirstName);
	const string lastname = STRSAFECONV(contact->LastName);
	const string phone1 = STRCONV(contact->Phone1);
	const string phone2 = STRSAFECONV(contact->Phone2);
	const string cellphone = STRSAFECONV(contact->Cellphone);
	const string email = STRSAFECONV(contact->Email);
	const string facebook = STRSAFECONV(contact->Facebook);
	const string linkedin = STRSAFECONV(contact->LinkedIn);

	ostringstream osql;
	if (DBM.HasData(CUSTOMERORGCONTACTS, "_id = '" + id + "'"))
	{
		osql << "update " << CUSTOMERORGCONTACTS << " SET "
			<< "state = " << (int)DataState::Edited << ", "
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
			<< "_id, state, corgccid, firstname, lastname, phone1, phone2, cellphone, email, facebook, linkedin ) "
			<< " VALUES ( "
			<< "'" << id << "', " << (int)DataState::Added << ", '" << corgccid << "', '" << firstname << "', '" << lastname
			<< "', '" << phone1 << "', '" << phone2 << "', '" << cellphone << "', '" << email << "', '"
			<< facebook << "', '" << linkedin << "' "
			<< ");";
	}

	auto ret = DBM.ExecSqlNative(osql.str());
	if (!DBM.IsSqlOk(ret))
		return MessageIds::DBUpdateFailed;
	contact->ObjectState = DataState::Unmodified;
	return MessageIds::Success;
}

task<MessageIds> scwin::scLocalDataService::UpdateCustomerContactAsync(String^ costCenterId, CustomerContact ^ contact)
{
	if (contact->ObjectState == DataState::Deleted)
	{
		return DeleteCustomerContactAsync(costCenterId, contact->Id);
	}

	return create_task([this, costCenterId, contact]() -> MessageIds
	{
		return UpdateCustomerContact(costCenterId, contact);
	});
}

MessageIds scwin::scLocalDataService::DeleteCustomerContact(String ^ costCenterId, String ^ contactId)
{
	return MessageIds::NotImplemented;
}

task<MessageIds> scwin::scLocalDataService::DeleteCustomerContactAsync(String ^ costCenterId, String ^ contactId)
{
	return create_task([this, costCenterId, contactId]()
	{
		return MessageIds::NotImplemented;
	});
}

IMap<String^, Address^>^ scwin::scLocalDataService::GetCostCenterAddresses(String ^ costCenterId)
{
	auto map = ref new Map<String^, Address^>();
	ostringstream osql;
	//		  0		1		2	3		4		5	6			7		8
	// a.* = _id, _rev, state, line1, line2, city, province, country, postalcode
	osql << "select a.*, '" << MAILING << "' from " << ADDRESSES << " a join " << CORGCOSTCENTER << " x on a._id = x.mailaddr where x._id = '" << STRCONV(costCenterId) << "' "
		<< "union "
		<< "select a.*, '" << SHIPPING << "' from " << ADDRESSES << " a join " << CORGCOSTCENTER << " x on a._id = x.shipaddr where x._id = '" << STRCONV(costCenterId) << "' "
		<< "union "
		<< "select a.*, '" << BILLING << "' from " << ADDRESSES << " a join " << CORGCOSTCENTER << " x on a._id = x.billaddr where x._id = '" << STRCONV(costCenterId) << "';";
	auto ret = DBM.ExecSqlNative(osql.str(), [this, map](void* p, int argc, char** argv, char** azColName)
	{
		//IMPORTANT, for performance reason the position of mailing, billing in the sql is hard-coded below. See also BuildAddress method.
		String^ addrType = STRCONV(argv[9]);
		auto address = BuildAddress(argv);
		map->Insert(addrType, address);
		return SQLITE_OK;
	});
	return map;
}

IAsyncOperation<IMap<String^, Address^>^>^ scwin::scLocalDataService::GetCostCenterAddressesAsync(String^ costCenterId)
{
	return create_async([this, costCenterId]()->IMap<String^, Address^>^
	{
		return GetCostCenterAddresses(costCenterId);
	});
}

Address ^ scwin::scLocalDataService::GetCostCenterAddress(String ^ costCenterId, String ^ addressType)
{
	ostringstream osql;
	if (addressType == MAILING)
	{
		osql << "select a.* from " << ADDRESSES << " a join " << CORGCOSTCENTER << " x on a._id = x.mailaddr where x._id = '" << STRCONV(costCenterId) << "'; ";
	}
	else if (addressType == SHIPPING)
	{
		osql << "select a.* from " << ADDRESSES << " a join " << CORGCOSTCENTER << " x on a._id = x.shipaddr where x._id = '" << STRCONV(costCenterId) << "'; ";
	}
	else
	{
		osql << "select a.* from " << ADDRESSES << " a join " << CORGCOSTCENTER << " x on a._id = x.billaddr where x._id = '" << STRCONV(costCenterId) << "'; ";
	}
	Address^ addr = nullptr;
	auto ret = DBM.ExecSqlNative(osql.str(), [this, &addr](void* p, int argc, char** argv, char** azColName)
	{
		//IMPORTANT, for performance reason the position of mailing, billing in the sql is hard-coded below. See also BuildAddress method.
		addr = BuildAddress(argv);
		return SQLITE_OK;
	});
	return addr;
}

IAsyncOperation<Address^>^ scwin::scLocalDataService::GetCostCenterAddressAsync(String ^ costCenterId, String ^ addressType)
{
	return create_async([this, costCenterId, addressType]() -> Address^
	{
		return GetCostCenterAddress(costCenterId, addressType);
	});
}

MessageIds scwin::scLocalDataService::UpdateCostCenterAddress(String ^ costCenterId, String ^ addressType, Address ^ address)
{
	String^ id = address->Id;
	const string line1 = STRSAFECONV(address->Line1);
	const string line2 = STRSAFECONV(address->Line2);
	const string city = STRSAFECONV(address->City);
	const string province = STRSAFECONV(address->Province);
	const string postalcode = STRSAFECONV(address->PostalCode);
	const string country = STRSAFECONV(address->Country);

	DBM.BeginTrans();

	ostringstream osql;
	std::string  crit = "_Id='" + STRCONV(id->Data()) + "'";
	if (DBM.HasData(ADDRESSES, crit))
	{
		osql << "UPDATE " << ADDRESSES << " SET ";
		osql << "state = " << (int)DataState::Edited << ", ";
		osql << "line1 = '" << line1 << "', ";
		osql << "line2 = '" << line2 << "', ";
		osql << "city = '" << city << "', ";
		osql << "province = '" << province << "', ";
		osql << "postalcode = '" << postalcode << "', ";
		osql << "country = '" << country << "' ";
		osql << "where _id = '" << STRCONV(id) << "';";
	}
	else
	{
		osql << "INSERT INTO " << ADDRESSES << " ";
		osql << "( _id, state, line1, line2, city, province, postalcode, country )";
		osql << " VALUES (";
		osql << "'" << STRCONV(id) << "', " << (int)DataState::Added << ", '" << line1 << "', '" << line2 << "',";
		osql << "'" << city << "', '" << province << "', '" << postalcode << "', '" << country << "' );";
	}

	// update costcenter.
	osql << "UPDATE " << CORGCOSTCENTER << " SET ";
	if (addressType == MAILING)
	{
		osql << "mailaddr = '" << STRCONV(address->Id) << "' ";
	}
	else if (addressType == BILLING)
	{
		osql << "billaddr = '" << STRCONV(address->Id) << "' ";
	}
	else
	{
		osql << "shipaddr = '" << STRCONV(address->Id) << "' ";
	}
	osql << " WHERE _id = '" << STRCONV(costCenterId) << "';";

	auto ret = DBM.ExecSqlNative(osql.str());
	if (!DBM.IsSqlOk(ret))
	{
		DBM.RollbackTrans();
		return MessageIds::DBUpdateFailed;
	}

	DBM.CommitTrans();

	return MessageIds::Success;
}

task<MessageIds> scwin::scLocalDataService::UpdateCostCenterAddressAsync(String ^ costCenterId, String ^ addressType, Address ^ address)
{
	return create_task([this, costCenterId, addressType, address]()->MessageIds
	{
		return UpdateCostCenterAddress(costCenterId, addressType, address);
	});
}

MessageIds scwin::scLocalDataService::DeleteCostCenterAddress(String ^ costCenterId, String ^ addressId)
{
	return MessageIds::NotImplemented;
}

task<MessageIds> scwin::scLocalDataService::DeleteCostCenterAddressAsync(String ^ costCenterId, String ^ addressId)
{
	return create_task([this, costCenterId, addressId]() -> MessageIds
	{
		return MessageIds::NotImplemented;
	});
}

IVector<FieldRep^>^ scwin::scLocalDataService::GetFieldReps(String ^ sqlcondition)
{
	throw ref new NotImplementedException();
}

IAsyncOperation<IVector<FieldRep^>^>^ scwin::scLocalDataService::GetFieldRepsAsync(String ^ sqlcondition)
{
	return create_async([this, sqlcondition]
	{
		return GetFieldReps(sqlcondition);
	});
}

FieldRep ^ scwin::scLocalDataService::GetFieldRep(String ^ id)
{
	throw ref new NotImplementedException();
	// TODO: insert return statement here
}

IAsyncOperation<FieldRep^>^ scwin::scLocalDataService::GetFieldRepAsync(String ^ id)
{
	return create_async([this, id]
	{
		return GetFieldRep(id);
	});
}

MessageIds scwin::scLocalDataService::UpdateFieldRep(FieldRep ^ fr)
{
	return MessageIds::NotImplemented;
}

task<MessageIds> scwin::scLocalDataService::UpdateFieldRepAsync(FieldRep ^ fr)
{
	return create_task([this, fr]()
	{
		return UpdateFieldRep(fr);
	});
}

MessageIds scwin::scLocalDataService::DeleteFieldRep(String ^ Id)
{
	return MessageIds::NotImplemented;
}

task<MessageIds> scwin::scLocalDataService::DeleteFieldRepAsync(String ^ Id)
{
	return create_task([this, Id]
	{
		return DeleteFieldRep(Id);
	});
}

IVector<GeneralStatus^>^ scwin::scLocalDataService::GetGeneralStatus(String ^ statusType)
{
	IVector<GeneralStatus^>^ output = ref new Vector<GeneralStatus^>();
	if (StringHelpers::IsNullOrEmpty(statusType))
		return output;
	ostringstream osql;
	if (statusType == ORDERSTATUS)
		osql << "select osid, privatename, publicname, desc from " << ORDERSTATUS << ";";
	else if (statusType == PAYMENTSTATUS)
		osql << "select psid, privatename, publicname, desc from " << PAYMENTSTATUS << ";";
	else if (statusType == PAYMENTINSTRUMENT)
		osql << "select pmid, privatename, publicname, desc from " << PAYMENTINSTRUMENT << ";";
	DBM.ExecSqlNative(osql.str(), [&output](void* p, int argc, char** argv, char**)
	{
		auto gstatus = ref new GeneralStatus(std::strtol(argv[0], nullptr, 10), STRCONV(argv[2]), STRCONV(argv[1]));
		gstatus->Description = STRCONV(argv[3]);
		output->Append(gstatus);
		return SQLITE_OK;
	});
	return output;
}

IAsyncOperation<IVector<GeneralStatus^>^>^ scwin::scLocalDataService::GetGeneralStatusAsync(String ^ statusType)
{
	return create_async([this, statusType]()
	{
		return GetGeneralStatus(statusType);
	});
}

GeneralStatus ^ scwin::scLocalDataService::GetGeneralStatus(String ^ statusType, int statusId)
{
	GeneralStatus^ gs = nullptr;
	ostringstream osql;
	if (statusType == ORDERSTATUS)
		osql << "select osid, privatename, publicname, desc from " << ORDERSTATUS << " where osid = " << statusId << ";";
	else if (statusType == PAYMENTSTATUS)
		osql << "select psid, privatename, publicname, desc from " << PAYMENTSTATUS << " where psid = " << statusId << ";";
	else if (statusType == PAYMENTINSTRUMENT)
		osql << "select pmid, privatename, publicname, desc from " << PAYMENTINSTRUMENT << " where pmid = " << statusId << ";";
	auto ret = DBSELECTORP(osql.str(), &gs)
	{
		gs = BuildStatus(argv);
		return SQLITE_OK;
	});
	return gs;
}

IVector<SalesOrder^>^ scwin::scLocalDataService::GetSalesOrders(const char* sqlcondition, const char* orderby)
{
	IVector<SalesOrder^>^ v = ref new Vector<SalesOrder^>();
	ostringstream osql;
	osql << "select _id, onumber, corgccid, batchno, odate, cccontactid, repid, ototalgross, disccode, "
		<< "discnum, disctype, taxpct, taxvalue, currencyid, ototalfinal, onotes, customerexpecteddate, owner "
		<< " from " << SALESORDERS;
		if (sqlcondition)
			osql << " where state != " << (int)DataState::Deleted << " and " << sqlcondition;
		if (orderby)
			osql << " order by " << orderby;
		osql << ";";
	auto ret = DBSELECTORP(osql.str(), &v)
	{
		v->Append(BuildOrder(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<SalesOrder^>^>^ scwin::scLocalDataService::GetSalesOrdersAsync(const char* sqlcondition, const char* orderby)
{
	return create_async([this, sqlcondition, orderby]()
	{
		return GetSalesOrders(sqlcondition, orderby);
	});
}

SalesOrder ^ scwin::scLocalDataService::GetSalesOrder(String ^ orderId)
{
	ostringstream osql; 
	//TODO
	osql << "select _id, onumber, corgccid, batchno, odate, cccontactid, repid, ototalgross, "
		<< "disccode, discnum, disctype, taxpct, taxvalue, currencyid, ototalfinal, onotes, customerexpecteddate, owner "
		<< " from " << SALESORDERS
		<< " where _id = '" << STRCONV(orderId) << "';";
	SalesOrder^ order = nullptr;
	auto ret = DBSELECTORP(osql.str(), &order)
	{
		order = BuildOrder(argv);
		return SQLITE_OK;
	});
	return order;
}

IAsyncOperation<SalesOrder^>^ scwin::scLocalDataService::GetSalesOrderAsync(String ^ orderId)
{
	return create_async([this, orderId]()
	{
		return GetSalesOrderAsync(orderId);
	});
}

IVector<SaleOrderDetails^>^ scwin::scLocalDataService::GetSalesOrderDetailList(String ^ orderId)
{
	ostringstream ostr;
	ostr << "select _id, soid, pid, pqty, unitprice, grossprice, pdisc, pdisctype, "
		<< "netprice, isexpedited, customerexpecteddate, owner "
		<< " from " << SALESORDERDETAILS << " where state != " << (int)DataState::Deleted << " and soid = '" << STRCONV(orderId) << "';";
	auto v = ref new Vector<SaleOrderDetails^>();
	auto ret = DBSELECTORP(ostr.str(), &v)
	{
		v->Append(BuildOrderDetails(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<SaleOrderDetails^>^>^ scwin::scLocalDataService::GetSalesOrderDetailListAsync(String ^ orderId)
{
	// TODO: insert return statement here
	return create_async([this, orderId]() -> IVector<SaleOrderDetails^>^
	{
		return GetSalesOrderDetailList(orderId);
	});
}

#define ORDERSTATUSHISTORYFIELDDEFS " _id, soid, ostatus, ostatusdate, notes, createdby "

IVector<SalesOrderStatusHistory^>^ scwin::scLocalDataService::GetOrderStatusHistory(String ^ orderId)
{
	ostringstream ostr;
	ostr << "select " << ORDERSTATUSHISTORYFIELDDEFS << " from " << SALESORDERSTATUSHISTORY << " where state != " << (int)DataState::Deleted << " and soid = '" << STRCONV(orderId) << "';";
	auto v = ref new Vector<SalesOrderStatusHistory^>();
	auto ret = DBSELECTORP(ostr.str(), &v)
	{
		v->Append(BuildOrderStatusHistory(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<SalesOrderStatusHistory^>^>^ scwin::scLocalDataService::GetOrderStatusHistoryAsync(String ^ orderId)
{
	return create_async([this, orderId]()
	{
		return GetOrderStatusHistory(orderId);
	});
}

#define ORDERPAYMENTHISTORYFIELDDEFS " _id , soid , paymentstatus , paymentinstrument , paymentdate , currency , paidamount , paydetails "

IVector<SalesOrderPaymentHistory^>^ scwin::scLocalDataService::GetOrderPaymentHistory(String ^ orderId)
{
	ostringstream ostr;
	ostr << "select " << ORDERPAYMENTHISTORYFIELDDEFS << " from " << SALESORDERPAYMENTHISTORY << " where state != " << (int)DataState::Deleted << " and soid = '" << STRCONV(orderId) << "';";
	auto v = ref new Vector<SalesOrderPaymentHistory^>();
	auto ret = DBSELECTORP(ostr.str(), &v)
	{
		v->Append(BuildOrderPaymentStatusHistory(argv));
		return SQLITE_OK;
	});
	return v;
}

IAsyncOperation<IVector<SalesOrderPaymentHistory^>^>^ scwin::scLocalDataService::GetOrderPaymentHistoryAsync(String ^ orderId)
{
	return create_async([this, orderId]()
	{
		return GetOrderPaymentHistory(orderId);
	});
}

MessageIds scwin::scLocalDataService::SaveSalesOrder(
	SalesOrder ^ order, 
	IVector<SaleOrderDetails^>^ orderDets, 
	SalesOrderStatusHistory ^ currentStatus,
	SalesOrderPaymentHistory^ currentPaymentStatus)
{

	if (order == nullptr || orderDets == nullptr || currentStatus == nullptr || currentPaymentStatus == nullptr)
	{
		return MessageIds::InvalidArgumentError;
	}

	if (currentStatus->OrderStatus == nullptr) 
		return MessageIds::MissingPropertyError;
	if (currentPaymentStatus->PaymentStatus == nullptr) 
		return MessageIds::MissingPropertyError;
	if (currentPaymentStatus->PaymentInstrument == nullptr) 
		return MessageIds::MissingPropertyError;

	ostringstream osql;
	DBM.BeginTrans();
	try
	{

		//CREATE TABLE salesorder(_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, onumber TEXT CONSTRAINT CatNameUnique UNIQUE ON CONFLICT FAIL, 
		// corgccid CHAR(32), batchno TEXT, odate NUMBER, cccontactid CHAR(32), repid TEXT, ototalgross REAL NOT NULL, 
		// disccode TEXT, discnum REAL, disctype TEXT, taxpct REAL, taxvalue REAL, currencyid TEXT, ototalfinal REAL NOT NULL, onotes TEXT, customerexpecteddate NUMBER, owner TEXT)
		
		String^ owner = GetCurrentUser()->Name;

		DataState state = DBM.HasData(SALESORDERS, "_id = '" + STRCONV(order->Id) + "'") ? DataState::Edited : DataState::Added;
		osql << "insert or replace into " << SALESORDERS
			<< "("
			<< "_id, state, onumber, corgccid, batchno, odate, cccontactid, repid, ototalgross, disccode, discnum, disctype, taxpct, taxvalue, currencyid, "
			<< "ototalfinal, onotes, customerexpecteddate, owner "
			<< ")"
			<< " VALUES ("
			<< "'" << STRCONV(order->Id) << "', " << (int)state << ", '" << STRSAFECONV(order->OrderNumber) << "', '" << STRCONV(order->CostCenterId) << "', '"
			<< STRCONV(order->BatchNumber) << "', " << order->DateAndTime->UnixTime << ", '" << STRCONV(order->ContactId) << "', '" << STRCONV(order->RepId) << "', "
			<< order->GrossAmount << ", '" << STRSAFECONV(order->DiscountCode) << "', " << order->Discount << ", '" << (int)order->DiscountType << "', "
			<< order->TaxPct << ", 0., '" << STRSAFECONV(order->CurrencyId) << "', " << order->FinalTotal << ", '" << STRSAFECONV(order->Notes) << "', " << order->CustomerExpectedDate->UnixTime << ", '"
			<< STRCONV(owner) << "' "
			<< ");";

		// insert order status history
		osql << "insert or replace into " << SALESORDERSTATUSHISTORY << " ( "
			<< "_id , state, soid, ostatus, ostatusdate, notes, createdby "
			<< ") "
			<< " values ( '"
			<< STRCONV(currentStatus->Id) << "', " << (int)DataState::Added << ", '" << STRCONV(order->Id) << "', " << currentStatus->OrderStatus->Id << ", " 
			<< currentStatus->OrderStatusDate->UnixTime << ", '" << STRSAFECONV(currentStatus->Notes) << "', '" << STRSAFECONV(currentStatus->CreatedBy) << "' "
			<< ");";
		// insert payment status history
		auto payDate = currentPaymentStatus->PaymentDate == nullptr ? -1 : currentPaymentStatus->PaymentDate->UnixTime;
		osql << "insert or replace into " << SALESORDERPAYMENTHISTORY << " ( "
			<< "_id, state, soid, paymentstatus, paymentinstrument, paymentdate, currency, paidamount, paydetails ) "
			<< " values ( '"
			<< STRCONV(currentPaymentStatus->Id) << "', " << (int)DataState::Added << ", '" << STRCONV(order->Id) << "', "
			<< currentPaymentStatus->PaymentStatus->Id << ", " << currentPaymentStatus->PaymentInstrument->Id << ", " << payDate << ", '"
			<< STRSAFECONV(currentPaymentStatus->CurrencyId) << "', " << currentPaymentStatus->PayAmount << ", '" << STRSAFECONV(currentPaymentStatus->PayDetails) << "' "
			<< " ); ";

		// remove items not in the collection.
		string availableids;
		for (auto odet : orderDets)
		{
			availableids += "'" + STRCONV(odet->Id) + "',";
			state = DBM.HasData(SALESORDERDETAILS, "_id = '" + STRCONV(odet->Id) + "'") ? DataState::Edited : DataState::Added;
			auto oDetPayDate = (currentPaymentStatus->PaymentDate == nullptr ? payDate : currentPaymentStatus->PaymentDate->UnixTime);
			if (!odet->IsExpedited)
				oDetPayDate = -1;
			osql << "insert or replace into " << SALESORDERDETAILS
				<< "("
				<< "_id, state, soid, pid, pqty, unitprice, grossprice, pdisc, pdisctype, netprice, isexpedited, customerexpecteddate, owner "
				<< ")"
				<< " values ('"
				<< STRCONV(odet->Id) << "', " << (int)state << ", '" << STRCONV(order->Id) << "', '" << STRCONV(odet->ProductId) << "', " << odet->Quantity << ", " 
				<< odet->UnitPrice << ", " << odet->Gross << ", " << odet->Discount << ", " << (int)odet->DiscountType << ", " << odet->Net << ", "
				<< (odet->IsExpedited ? 1 : 0) << ", " << oDetPayDate << ", '" << STRSAFECONV(owner) << "' "
				<< ");";

			// salesorderitem status history will be done if absolutely necessary.
			// CREATE TABLE salesorderitemstatushistory (_id CHAR(32) PRIMARY KEY, _rev TEXT, state INTEGER NOT NULL DEFAULT 0, ositemid CHAR(32) NOT NULL, oslid CHAR(32) NOT NULL, osstatus INTEGER NOT NULL, paymentstatus INTEGER, notes TEXT, createdby TEXT NOT NULL, UNIQUE (ositemid, oslid, state) )
		}
		// remove last comma
		if (!availableids.empty())
		{
			availableids = availableids.substr(0, availableids.size() - 1);
			osql << "update " << SALESORDERDETAILS << " set state = " << (int)DataState::Deleted << " where soid == '" << STRCONV(order->Id) << "' and _id not in (" << availableids << ");";
		}


		// TODO INVENTORY LOCKING AND UPDATING. INVENTORY LOCKING HAPPENS DURING ORDER CREATION AND UNLOCKED WHEN ORDER IS CANCELLED OR REVERTED
		// LOCKED STATE IS CONVERTED TO CUSTOMER ORDER STATE WHEN ORDER IS SAVED.
		// for these call UpdateInventory method.

		auto ret = DBM.ExecSql(osql.str());
		if (!DBM.IsSqlOk(ret))
		{
			DBM.RollbackTrans();
			return MessageIds::DBUpdateFailed;
		}
		order->ObjectState = DataState::Unmodified;
		for (auto odet : orderDets)
		{
			odet->ObjectState = DataState::Unmodified;
		}
		currentStatus->ObjectState = DataState::Unmodified;
		currentPaymentStatus->ObjectState = DataState::Unmodified;
		DBM.CommitTrans();
		return MessageIds::Success;
	}
	catch (const std::exception& ex)
	{
		DBM.RollbackTrans();
		return MessageIds::DBUpdateFailed;
	}
}

task<MessageIds> scwin::scLocalDataService::SaveSalesOrderAsync(
	SalesOrder ^ order, 
	IVector<SaleOrderDetails^>^ orderDets, 
	SalesOrderStatusHistory ^ currentStatus,
	SalesOrderPaymentHistory^ currentPaymentStatus)
{
	return create_task([=]()
	{
		return SaveSalesOrder(order, orderDets, currentStatus, currentPaymentStatus);
	});
}

SalesOrderStatusHistory ^ scwin::scLocalDataService::GetLatestOrderStatus(String ^ orderId)
{
	ostringstream osql;
	osql << "select " << ORDERSTATUSHISTORYFIELDDEFS << " from  " << SALESORDERSTATUSHISTORY 
		<< " where soid = '" << STRCONV(orderId) << "' "
		<< "order by ostatusdate desc limit 1;";
	SalesOrderStatusHistory^ hist = nullptr;
	auto ret = DBSELECTORP(osql.str(), &hist)
	{
		hist = BuildOrderStatusHistory(argv);
		return SQLITE_OK;
	});
	return hist;
}

SalesOrderPaymentHistory ^ scwin::scLocalDataService::GetLatestOrderPaymentStatus(String ^ orderId)
{
	ostringstream osql;
	osql << "select " << ORDERPAYMENTHISTORYFIELDDEFS << " from  " << SALESORDERPAYMENTHISTORY
		<< " where soid = '" << STRCONV(orderId) << "' order by paymentdate desc limit 1;";
	SalesOrderPaymentHistory^ hist = nullptr;
	auto ret = DBSELECTORP(osql.str(), &hist)
	{
		auto h = BuildOrderPaymentStatusHistory(argv);
		hist = h;
		return SQLITE_OK;
	}
	);
	return hist;
}

#define INVENTORYLOCATIONFIELDS " _id , _rev , state , location , code , contactid , addressid "

IObservableVector<ProductInventoryLocation^>^ scwin::scLocalDataService::GetInventoryLocations()
{
	ostringstream osql;
	osql << "select " << INVENTORYLOCATIONFIELDS << " from " << INVENTORYLOCATION << " order by location;";
	Vector<ProductInventoryLocation^>^ v = ref new Vector<ProductInventoryLocation^>();
	auto ret = DBSELECTORP(osql.str(), &v)
	{
		auto l = BuildInventoryLocation(argv);
		v->Append(l);
		return SQLITE_OK;
	}
	);
	return v;
}

IAsyncOperation<IObservableVector<ProductInventoryLocation^>^>^ scwin::scLocalDataService::GetInventoryLocationsAsync()
{
	return create_async([this]()
	{
		return GetInventoryLocations();
	});
}

ProductInventoryLocation ^ scwin::scLocalDataService::GetInventoryLocation(String ^ locationId)
{
	ostringstream osql;
	osql << "select " << INVENTORYLOCATIONFIELDS << " from " << INVENTORYLOCATION << " where _id = '" << STRCONV(locationId) << "';";
	ProductInventoryLocation^ location = nullptr;
	auto ret = DBSELECTORP(osql.str(), &location)
	{
		auto l = BuildInventoryLocation(argv);
		location = l;
		return SQLITE_OK;
	});
	return location;
}
IAsyncOperation<ProductInventoryLocation^>^ scwin::scLocalDataService::GetInventoryLocationAsync(String ^ locationId)
{
	return create_async([this, locationId]()
	{
		return GetInventoryLocation(locationId);
	});
}

MessageIds scwin::scLocalDataService::SaveInventoryLocation(ProductInventoryLocation ^ location)
{
	if (!location->IsValid)
		throw CreateException(MessageIds::MissingValuesError);

	DBM.BeginTrans();
	try
	{
		auto addr = location->Address;
		if (addr != nullptr)
		{
			// check address already exists
			UpdateAddress(addr);
		}
		ostringstream osql;
		osql << "insert or replace into " << INVENTORYLOCATION << " ( _id , state , location , code , contactid, addressid  ) "
			<< " VALUES ( "
			<< " '" << STRCONV(location->Id) << "', " << (int)location->ObjectState << ", '" << STRCONV(location->Name) << "', "
			<< " '" << STRCONV(location->Code) << "', "
			<< " '" << STRCONV(location->ContactId) << "', ";
		if (addr != nullptr)
			osql << "'" << STRCONV(location->Address->Id) << "' ";
		else
			osql << "'' ";
		osql << " ) ";
		auto ret = DBM.ExecSql(osql.str());
		if (!DBM.IsSqlOk(ret))
		{
			DBM.RollbackTrans();
			return MessageIds::DBUpdateFailed;
		}
		location->ObjectState = DataState::Unmodified;
		location->Address->ObjectState = DataState::Unmodified;
		DBM.CommitTrans();
		return MessageIds::Success;
	}
	catch (const std::exception& ex)
	{
		DBM.RollbackTrans();
		return MessageIds::DBUpdateFailed;
	}
}

MessageIds scwin::scLocalDataService::UpdateProductInventory(String ^ productId, uint64 quantity, String ^ locationId, InventoryMoveDirection movementType)
{
	return MessageIds::NotImplemented;
}

#define APPBUSINESSORGFIELDS "_id, _rev, state, objdata "

AppBusinessOrg ^ scwin::scLocalDataService::GetAppBusinessOrg()
{
	ostringstream osql;
	osql << "select " << APPBUSINESSORGFIELDS << " from " << APPBUSINESSORG << " limit 1;";
	AppBusinessOrg^ org = nullptr;
	auto ret = DBSELECTORP(osql.str(), &org)
	{
		// get the json data
		JsonObject^ obj;
		if (JsonObject::TryParse(STRCONV(argv[3]), &obj))
		{
			org = ref new AppBusinessOrg(STRCONV(argv[0]));
			org->CanRaiseEvents = false;
			auto jhelpDesk = obj->GetNamedObject("helpdesk", nullptr);
			if (jhelpDesk != nullptr)
			{
				auto helpDesk = ref new HelpDesk();
				helpDesk->Email = jhelpDesk->GetNamedString("email", L"");
				helpDesk->Phone = jhelpDesk->GetNamedString("phone", L"");
				helpDesk->Url = jhelpDesk->GetNamedString("url", L"");
				org->HelpDesk = helpDesk;
			}
			auto jtechContact = obj->GetNamedObject("techcontact");
			if (jtechContact != nullptr)
			{
				auto techCont = ref new TechContact();
				techCont->Email = jtechContact->GetNamedString("email", L"");
				techCont->Name = jtechContact->GetNamedString("name", L"");
				techCont->Phone = jtechContact->GetNamedString("phone", L"");
				org->TechContact = techCont;
			}
			org->Name = obj->GetNamedString("name", L"");
			org->Website = obj->GetNamedString("website", L"");
			org->AddressId = obj->GetNamedString("addressid", nullptr);
			org->LogoImageId = obj->GetNamedString("logimageid", nullptr);
			org->CanRaiseEvents = true;
		}
		return SQLITE_OK;
	});
	if (org == nullptr)
		org = ref new AppBusinessOrg();
	return org;
}

IAsyncOperation<AppBusinessOrg^>^ scwin::scLocalDataService::GetAppBusinessOrgAsync()
{
	return create_async([=]()
	{
		return GetAppBusinessOrg();
	});
}

MessageIds scwin::scLocalDataService::SaveAppBusinessOrg(AppBusinessOrg ^ org)
{
	if (!org)
	{
		return MessageIds::InvalidArgumentError;
	}

	JsonObject^ obj = ref new JsonObject();
	obj->SetNamedValue("name", JsonValue::CreateStringValue(org->Name));
	obj->SetNamedValue("logoimageid", JsonValue::CreateStringValue(org->LogoImageId));
	obj->SetNamedValue("addressid", JsonValue::CreateStringValue(org->AddressId));
	obj->SetNamedValue("website", JsonValue::CreateStringValue(org->Website));
	if (org->HelpDesk != nullptr)
	{
		JsonObject^ jhelpdesk = ref new JsonObject();
		jhelpdesk->SetNamedValue("email", JsonValue::CreateStringValue(org->HelpDesk->Email));
		jhelpdesk->SetNamedValue("phone", JsonValue::CreateStringValue(org->HelpDesk->Phone));
		jhelpdesk->SetNamedValue("url", JsonValue::CreateStringValue(org->HelpDesk->Url));
		obj->SetNamedValue("helpdesk", jhelpdesk);
	}
	else
		obj->Remove("helpdesk");
	if (org->TechContact != nullptr)
	{
		JsonObject^ jtechcontact = ref new JsonObject();
		jtechcontact->SetNamedValue("email", JsonValue::CreateStringValue(org->TechContact->Email));
		jtechcontact->SetNamedValue("name", JsonValue::CreateStringValue(org->TechContact->Name));
		jtechcontact->SetNamedValue("phone", JsonValue::CreateStringValue(org->TechContact->Phone));
		obj->SetNamedValue("techcontact", jtechcontact);
	}
	else
		obj->Remove("helpdesk");
	DataState state = org->ObjectState;
	ostringstream osql;
	osql << "insert or replace into " << APPBUSINESSORG
		<< " (_id, state, objdata )"
		<< " VALUES "
		<< "( '" << STRCONV(org->Id) << "', " << (int)state << ", '" << STRSAFECONV(obj->ToString()) << "' );";

	MessageIds retCode = MessageIds::DBUpdateFailed;
	try
	{
		auto status = DBM.ExecSqlNative(osql.str());
		if (status == SQLITE_OK)
		{
			org->ObjectState = DataState::Unmodified;
			return MessageIds::Success;
		}
		else
		{
			return retCode;
		}
	}
	catch (Exception^ ex)
	{
		LogException(StringHelpers::Convert(ex->Message).c_str());
		return retCode;
	}
}

task<MessageIds> scwin::scLocalDataService::SaveAppBusinessOrgAsync(AppBusinessOrg ^ org)
{
	return create_task([=]()
	{
		return SaveAppBusinessOrg(org);
	});
}

#ifdef DEBUG

void scwin::scLocalDataService::PrintDbState()
{
	DBM.GetDbHandler().PrintState();
}

#endif

