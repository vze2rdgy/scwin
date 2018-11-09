#include "pch.h"
#include "IsTenFootTrigger.h"
#include "SysInfoHelpers.h"

using namespace scwin;

IsTenFootTrigger::IsTenFootTrigger()
{
	this->IsTenFoot = true;
}

bool IsTenFootTrigger::IsTenFoot::get()
{
	return this->isTenFootRequested;
}

void IsTenFootTrigger::IsTenFoot::set(bool val)
{
	this->isTenFootRequested = val;
	this->SetActive(utils::SysInfoHelpers::IsTenFootExperience() == isTenFootRequested);
}
