#include "pch.h"
#include "CustomerDtosGroupedBindable.h"
#include "cpplinq.hpp"
#include <algorithm>

////////////////////////////////// CustomerDtosGroupedBindable ///////////////////////////////////

CustomerDtosGroupedBindable::CustomerDtosGroupedBindable()
{
	corg = ref new CustomerOrg();
	defCostCenter = ref new CostCenterDtosGroupedBindable(ref new COrgCostCenter());
	costcenters = ref new Vector<CostCenterDtosGroupedBindable^>();
}

scwin::CustomerDtosGroupedBindable::CustomerDtosGroupedBindable(CustomerOrg ^ org)
{
	Organization = org;
	// asynchronously populate other data...
	BuildThisAsync(org);
}

task<MessageIds> scwin::CustomerDtosGroupedBindable::ExecuteSaveCommand(Object ^ param)
{
	// organize objects
	// Save Org
	// Save Default Cost Center
		// Save Addresses
		// Save Contacts
	// Save Other Cost Centers
		// Save Addresses
		// Save Contacts
	WeakReference wr(this);
	auto saveTask = create_task([wr]() -> MessageIds
	{

		auto ths = wr.Resolve<CustomerDtosGroupedBindable>();
		auto lds = scLocalDataService::Instance();
		// begin trans
		if (!lds->StartTransaction())
		{
			return MessageIds::DBTransactionBeginFailed;
		}

		auto msgId = lds->UpdateCustomerOrganization(ths->Organization);

		if (HASFAILED(msgId))
		{
			lds->CancelTransaction();
			return msgId;
		}

		msgId = lds->UpdateCustomerCostCenter(ths->DefaultCostCenter->CostCenter);
		if (HASFAILED(msgId))
		{
			lds->CancelTransaction();
			return msgId;
		}

		auto updcostCenterContacts = [lds](CostCenterDtosGroupedBindable^ cc) -> MessageIds
		{
			if (cc->Contacts != nullptr)
			{
				for (auto c : cc->Contacts)
				{
					c->Value->CostCenterId = cc->CostCenter->Id;
					if (!c->Value->IsValid || HASFAILED(lds->UpdateCustomerContact(cc->CostCenter->Id, c->Value)))
					{
						return MessageIds::MissingValuesError;
					}
				}
			}
			return MessageIds::Success;
		};

		auto updcostCenterAddresses = [lds](CostCenterDtosGroupedBindable^ cc) -> MessageIds
		{
			if (cc->Addresses != nullptr)
			{
				for (auto a : cc->Addresses)
				{
					if (!a->Value->IsValid || HASFAILED(lds->UpdateCostCenterAddress(cc->CostCenter->Id, a->Key, a->Value)))
					{
						return MessageIds::MissingValuesError;
					}
				}
			}
			return MessageIds::Success;
		};

		msgId = updcostCenterContacts(ths->DefaultCostCenter);
		if (HASFAILED(msgId))
		{
			lds->CancelTransaction();
			return msgId;
		}

		msgId = updcostCenterAddresses(ths->DefaultCostCenter);
		if (HASFAILED(msgId))
		{
			lds->CancelTransaction();
			return msgId;
		}

		if (ths->CostCenters != nullptr)
		{
			for (auto cc : ths->CostCenters)
			{
				cc->CostCenter->IsDefault = false;
				msgId = lds->UpdateCustomerCostCenter(cc->CostCenter);
				if (HASFAILED(msgId))
				{
					lds->CancelTransaction();
					return msgId;
				}
				msgId = updcostCenterContacts(cc);
				if (HASFAILED(msgId))
				{
					lds->CancelTransaction();
					return msgId;
				}
				msgId = updcostCenterAddresses(cc);
				if (HASFAILED(msgId))
				{
					lds->CancelTransaction();
					return msgId;
				}
			}
		}

		lds->CommitTransaction();

#ifdef DEBUG
		lds->PrintDbState();
#endif

		return msgId;
	});

	return saveTask;
}

bool scwin::CustomerDtosGroupedBindable::CanExecuteSaveCommand(Object ^ param)
{
	auto canexecute = this->IsValid;
	if (!canexecute)
	{
		PubSubService::Instance->Publish(this, PubSubMessageIds::ApplicationErrorStatusMessage, "Not all required information is provided. The required items are marked with a red asterisk (*)");
	}
	return canexecute;
}

void scwin::CustomerDtosGroupedBindable::OnOrgPropertyChanged(Object ^ sender, PropertyChangedEventArgs ^ args)
{
	if (args->PropertyName == "Code")
	{
		CustomerOrg^ obj = safe_cast<CustomerOrg^>(sender);
		if (StringHelpers::IsNullOrEmpty(this->DefaultCostCenter->CostCenter->Code))
			this->DefaultCostCenter->CostCenter->Code = obj->Code;
	}
}

void scwin::CustomerDtosGroupedBindable::AddCostCenter()
{
	if (CostCenters == nullptr)
		CostCenters = ref new Vector<CostCenterDtosGroupedBindable^>();
	CostCenterDtosGroupedBindable^ ccbounded = ref new CostCenterDtosGroupedBindable();
	ccbounded->CostCenter = ref new COrgCostCenter();
	ccbounded->CostCenter->ParentOrgId = Organization->Id;
	ccbounded->CostCenter->IsDefault = false;
	ccbounded->CostCenter->IsActive = true;
	ccbounded->Addresses = ref new Vector<AddressKeyValuePair^>();
	ccbounded->Contacts = ref new Vector<CustomerContactKeyValuePair^>();
	ccbounded->Position = CostCenters->Size + 1;
	ccbounded->CostCenter->Code = scLocalDataService::Instance()->AutoGenerateCode(CORGCOSTCENTER);
	CostCenters->Append(ccbounded);
}

#include "HashingHelpers.h"

IAsyncOperation<CustomerDtosGroupedBindable^>^ scwin::CustomerDtosGroupedBindable::New()
{
	return create_async([]()
	{
		CustomerOrg^ org = ref new CustomerOrg();
		org->IsActive = true;
		CustomerDtosGroupedBindable^ bounded = ref new CustomerDtosGroupedBindable();
		bounded->Organization = org;
		bounded->SubHeader = "Create a new Organization and set up its cost centers and contacts.";
		bounded->CostCenters = ref new Vector<CostCenterDtosGroupedBindable^>();
		// set default cost center
		auto ccbounded = ref new CostCenterDtosGroupedBindable();
		bounded->DefaultCostCenter = ccbounded;
		ccbounded->CostCenter = ref new COrgCostCenter();
		ccbounded->CostCenter->ParentOrgId = org->Id;
		ccbounded->CostCenter->Name = "Default Cost Center";
		ccbounded->CostCenter->IsDefault = true;
		ccbounded->CostCenter->IsActive = true;
		ccbounded->Addresses = ref new Vector<AddressKeyValuePair^>();
		ccbounded->Contacts = ref new Vector<CustomerContactKeyValuePair^>();
		org->PropertyChanged += ref new PropertyChangedEventHandler(bounded, &CustomerDtosGroupedBindable::OnOrgPropertyChanged);
		org->Code = scLocalDataService::Instance()->AutoGenerateCode(CUSTOMERORGS);
		return bounded;
	});
}

void scwin::CustomerDtosGroupedBindable::BuildThisAsync(CustomerOrg^ org)
{
	WeakReference wr(this);
	create_task([wr, org]()
	{
		if (org == nullptr) return;
		// get the default costcenter
		auto lvs = scLocalDataService::Instance();
		auto ccs = lvs->GetCustomerCostCenters("corgid='" + org->Id + "'");
		if (ccs == nullptr || ccs->Size == 0) return;
		auto ths = wr.Resolve<CustomerDtosGroupedBindable>();
		// loop and add costcenters, if one of them is default, don't add it to the 
		// list, but set as default costcenter
		auto CostCenters = ref new Vector<CostCenterDtosGroupedBindable^>();
		for (auto cc : ccs)
		{
			auto ccwrap = ref new CostCenterDtosGroupedBindable(cc);
			if (cc->IsDefault)
			{
				ths->DefaultCostCenter = ccwrap;
			}
			else
			{
				CostCenters->Append(ccwrap);
				ccwrap->Position = ths->CostCenters->Size;
			}
		}
		ths->CostCenters = CostCenters;
	});
}

bool scwin::CustomerDtosGroupedBindable::SaveOrganization()
{
	auto lds = scLocalDataService::Instance();
	auto org = Organization;
	if (HASFAILED(lds->UpdateCustomerOrganization(org)))
	{
		PubSubService::Instance->PublishError(
			this, 
			"Failed to save organization. Please retry."
		);
		return false;
	}
	return true;
}

IAsyncOperation<bool>^ scwin::CustomerDtosGroupedBindable::SaveOrganizationAsync()
{
	return create_async([=]() -> bool
	{
		return SaveOrganization();
	});
}

bool CustomerDtosGroupedBindable::IsValid::get()
{
	isValid = true;
	// validation 
	// check if customerorg is valid
	if (corg == nullptr || !corg->IsValid)
	{
		isValid = false;
	}
	else
	{
		// by default the default costcenter must be provided apart from organization.
		// everything else such as address and contacts are optional (until business requirements change).
		if (DefaultCostCenter == nullptr || !DefaultCostCenter->CostCenter->IsValid)
		{
			return false;
		}
	}

	return isValid;
}

void CustomerDtosGroupedBindable::IsValid::set(bool val)
{
	if (val != isValid)
	{
		isValid = val;
		RaisePropertyChanged("IsValid");
	}
}

DEFINERAISEPROPCHANGEDBASIC(CustomerDtosGroupedBindable);


/////////////////////////////// CostCenterDtosGroupedBindable ///////////////////////////////

scwin::CostCenterDtosGroupedBindable::CostCenterDtosGroupedBindable()
{
	//EditLocation = ref new Command(
	//	ref new ExecuteAction(this, &CostCenterDtosGroupedBindable::ExecEditLocation),
	//	ref new CanExecuteFunc(this, &CostCenterDtosGroupedBindable::CanExecEditLocation)
	//);
	// initialize internal cc
	cc = ref new COrgCostCenter();
	addresses = ref new Vector<AddressKeyValuePair^>();
	contacts = ref new Vector<CustomerContactKeyValuePair^>();
}

scwin::CostCenterDtosGroupedBindable::CostCenterDtosGroupedBindable(COrgCostCenter ^ cc)
{
	this->cc = cc;
	BuildThisAsync();
	//Build();
}

bool scwin::CostCenterDtosGroupedBindable::Save()
{
	MessageIds status;
	auto lds = scLocalDataService::Instance();
	lds->StartTransaction();
	status = lds->UpdateCustomerCostCenter(CostCenter);
	if (status != MessageIds::Success)
		goto EXIT;
	if (MailingAddress->Value->IsValid)
		status = lds->UpdateCostCenterAddress(CostCenter->Id, MAILING, MailingAddress->Value);
	if (BillingAddress->Value->IsValid)
		status = lds->UpdateCostCenterAddress(CostCenter->Id, BILLING, BillingAddress->Value);
	if (ShippingAddress->Value->IsValid)
		status = lds->UpdateCostCenterAddress(CostCenter->Id, SHIPPING, ShippingAddress->Value);
	lds->CommitTransaction();
	return true;

EXIT:
	lds->CancelTransaction();
	PubSubService::Instance->PublishError(this, "Failed to save location with error : " + ref new String(GetMessage(status)));
	return false;
}

bool scwin::CostCenterDtosGroupedBindable::Cancel()
{
	return true;
}

void scwin::CostCenterDtosGroupedBindable::BuildThisAsync()
{
	create_task([=]() {Build(); });
}

void scwin::CostCenterDtosGroupedBindable::Build()
{
	using namespace cpplinq;
	auto ths = this; // wr.Resolve<CostCenterDtosGroupedBindable>();
	// contacts and addresses
	if (ths->cc == nullptr) return;
	COrgCostCenter^ cc = ths->cc;
	auto lvs = scLocalDataService::Instance();
	auto contacts = lvs->GetCustomerContacts("corgccid='" + cc->Id + "'");
	int incr = 1;
	ths->Contacts = ref new Vector<CustomerContactKeyValuePair^>(from(to_vector(contacts)) >> 
		select([this, &incr, cc](CustomerContact^ c)
	{
		return ref new CustomerContactKeyValuePair(incr++, c, this);
	}) >> to_vector());

	//for (auto c : contacts)
	//{
	//	if (ths->Contacts == nullptr)
	//		ths->Contacts = ref new Vector<CustomerContactKeyValuePair^>();
	//	ths->Contacts->Append(ref new CustomerContactKeyValuePair(ths->Contacts->Size + 1, c));
	//}

	auto addresses = lvs->GetCostCenterAddresses(cc->Id);
	auto Addresses = ref new Vector<AddressKeyValuePair^>();
	set<String^> keysset;
	map<String^, String^> sameaskeys;
	set<String^> allkeys = { MAILING, BILLING, SHIPPING };
	for (auto a : addresses)
	{
		keysset.insert(a->Key);
		sameaskeys.insert(make_pair(a->Value->Id, a->Key));
	}

	for (auto a : addresses)
	{
		auto wrapper = ref new AddressKeyValuePair(a->Key, a->Value, cc->Id);
		wrapper->AddressOptionSelected = sameaskeys[a->Value->Id];
		if (StringHelpers::IsNullOrEmpty(wrapper->AddressOptionSelected))
			wrapper->AddressOptionSelected = "None";
		Addresses->Append(wrapper);
		wrapper->CanDisplayDeleteButton = false;
	}
	vector<String^> diff;
	std::set_symmetric_difference(allkeys.cbegin(), allkeys.cend(), keysset.cbegin(), keysset.cend(), std::back_inserter(diff));

	// add an address object for missing address types
	for (auto key : diff)
	{
		auto wrapper = ref new AddressKeyValuePair(key, ref new Address(), cc->Id);
		wrapper->AddressOptionSelected = "None";
		wrapper->ShowAddressOptions = false;
		Addresses->Append(wrapper);
		wrapper->CanDisplayDeleteButton = false;
	}
}

AddressKeyValuePair^ CostCenterDtosGroupedBindable::MailingAddress::get()
{
	using namespace cpplinq;
	AddressKeyValuePair^ addr = from(to_vector(addresses)) >>
		where([](auto p) 
	{
		return (p->Key == MAILING);
	}) >> first_or_default();
	if (addr == nullptr)
	{
		addr = ref new AddressKeyValuePair(MAILING, ref new Address(), cc->Id);
		addresses->Append(addr);
	}
	return addr;
}

void CostCenterDtosGroupedBindable::MailingAddress::set(AddressKeyValuePair^ val)
{

}

AddressKeyValuePair^ CostCenterDtosGroupedBindable::BillingAddress::get()
{
	using namespace cpplinq;
	AddressKeyValuePair^ addr = from(to_vector(addresses)) >>
		where([](auto p)
	{
		return (p->Key == BILLING);
	}) >> first_or_default();
	if (addr == nullptr)
	{
		addr = ref new AddressKeyValuePair(BILLING, ref new Address(), cc->Id);
		addr->AddressOptionSelected = "None";
		addresses->Append(addr);
	}
	return addr;
}

void CostCenterDtosGroupedBindable::BillingAddress::set(AddressKeyValuePair^ val)
{
	// find index for billing in the address vector.
	// if the val->Key is BILLING, then simply insert at that location
	// else create a new addresskeyvaluepair with key = billing and then use same address from val parameter.
	auto v = to_vector(addresses);
	auto pos = std::find_if(v.cbegin(), v.cend(), [](auto p) { return p->Key == BILLING; }) - v.cbegin();
	if (pos >= 0 && pos < v.size())
	{
		if (val->Key == BILLING)
		{
			addresses->SetAt(pos, val);
		}
		else
		{
			auto avp = ref new AddressKeyValuePair(BILLING, val->Value, cc->Id);
			avp->AddressOptionSelected = val->Key;
			addresses->SetAt(pos, avp);
		}
	}
	RaisePropertyChanged(PROPERTYNAME(BillingAddress));
}

AddressKeyValuePair^ CostCenterDtosGroupedBindable::ShippingAddress::get()
{
	using namespace cpplinq;
	AddressKeyValuePair^ addr = from(to_vector(addresses)) >>
		where([](auto p)
	{
		return (p->Key == SHIPPING);
	}) >> first_or_default();
	if (addr == nullptr)
	{
		addr = ref new AddressKeyValuePair(SHIPPING, ref new Address(), cc->Id);
		addr->AddressOptionSelected = "None";
		addresses->Append(addr);
	}
	return addr;
}

void CostCenterDtosGroupedBindable::ShippingAddress::set(AddressKeyValuePair^ val)
{
	// find index for shipping in the address vector.
	// if the val->Key is shipping, then simply insert at that location
	// else create a new addresskeyvaluepair with key = shipping and then use same address from val parameter.
	auto v = to_vector(addresses);
	auto pos = std::find_if(v.cbegin(), v.cend(), [](auto p) { return p->Key == BILLING; }) - v.cbegin();
	if (pos >= 0 && pos < v.size())
	{
		if (val->Key == SHIPPING)
		{
			addresses->SetAt(pos, val);
		}
		else
		{
			auto avp = ref new AddressKeyValuePair(SHIPPING, val->Value, cc->Id);
			avp->AddressOptionSelected = val->Key;
			addresses->SetAt(pos, avp);
		}
	}
	RaisePropertyChanged(PROPERTYNAME(ShippingAddress));
}

bool CostCenterDtosGroupedBindable::IsValid::get()
{
	isValid = true;
	if (this->CostCenter == nullptr || !this->CostCenter->IsValid)
	{
		isValid = false;
	}
	return isValid;
}

void CostCenterDtosGroupedBindable::IsValid::set(bool val)
{
	if (val != isValid)
	{
		isValid = val;
		RaisePropertyChanged("IsValid");
	}
}



//void scwin::CostCenterDtosGroupedBindable::ExecEditLocation(Object ^ param)
//{
//
//}
//
//bool scwin::CostCenterDtosGroupedBindable::CanExecEditLocation(Object ^ param)
//{
//	return true;
//}

DEFINERAISEPROPCHANGEDBASIC(CostCenterDtosGroupedBindable)

