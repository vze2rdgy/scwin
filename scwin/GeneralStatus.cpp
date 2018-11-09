#include "pch.h"
#include "GeneralStatus.h"


GeneralStatus::GeneralStatus()
{
}

scwin::GeneralStatus::GeneralStatus(int id, String^ publicName, String^ privateName)
{
	Id = id;
	PrivateName = privateName;
	PublicName = publicName;
}
