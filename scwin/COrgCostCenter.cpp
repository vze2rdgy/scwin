#include "pch.h"
#include "COrgCostCenter.h"



COrgCostCenter::COrgCostCenter()
{
	this->id = SysInfoHelpers::NewCouchDbId();
	ObjectState = DataState::Added;
	CanRaiseEvents = true;
}

COrgCostCenter::COrgCostCenter(String^ id)
{
	this->id = id;
	ObjectState = DataState::Unmodified;
	CanRaiseEvents = true;
}

COrgCostCenter::~COrgCostCenter()
{

}

DEFINERAISEPROPCHANGED(COrgCostCenter)
