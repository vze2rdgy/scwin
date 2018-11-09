#include "pch.h"
#include "ModelKeyValuePairs.h"
#include "CustomerDtosGroupedBindable.h"

///////////// AddressKeyValuePair ////////////////////

scwin::AddressKeyValuePair::AddressKeyValuePair(String ^ key, Address ^ value, String^ ccid) :
	key(key), value(value), _ccid(ccid)
{
	if (key == BILLING)
	{
		_addressTypes = ref new Vector<String^>({ "None", MAILING, SHIPPING });
	}
	else if (key == SHIPPING)
	{
		_addressTypes = ref new Vector<String^>({ "None", MAILING, BILLING});
	}
	else
		_addressTypes = ref new Vector<String^>({ "None"});
}

DEFINERAISEPROPCHANGEDBASIC(AddressKeyValuePair);

///////////// CustomerContactKeyValuePair ////////////////////

scwin::CustomerContactKeyValuePair::CustomerContactKeyValuePair(
	UINT pos, 
	CustomerContact^ value, 
	CostCenterDtosGroupedBindable^ cc
	) : pos(pos), value(value)
{
	_cc = cc;
}

bool scwin::CustomerContactKeyValuePair::Save()
{
	auto msgId = scLocalDataService::Instance()->UpdateCustomerContact(_cc->CostCenter->Id, Value);
	if (msgId != MessageIds::Success)
	{
		PubSubService::Instance->PublishError(this, "Failed to save location contact with error " + ref new String(GetMessage(msgId)) + ". Please retry.");
		return false;
	}
	return true;
}

bool scwin::CustomerContactKeyValuePair::Cancel()
{
	return true;
}

DEFINERAISEPROPCHANGEDBASIC(CustomerContactKeyValuePair);
